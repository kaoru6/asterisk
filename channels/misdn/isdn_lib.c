/*
 * Chan_Misdn -- Channel Driver for Asterisk
 *
 * Interface to mISDN
 *
 * Copyright (C) 2004, Christian Richter
 *
 * Christian Richter <crich@beronet.com>
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License
 */

#include "isdn_lib_intern.h"
#include <mISDNuser/isdn_debug.h>


void misdn_join_conf(struct misdn_bchannel *bc, int conf_id);
void misdn_split_conf(struct misdn_bchannel *bc, int conf_id);

int queue_cleanup_bc(struct misdn_bchannel *bc) ;


struct misdn_stack* get_misdn_stack( void );


int misdn_lib_is_ptp(int port)
{
	struct misdn_stack *stack=get_misdn_stack();
	for ( ; stack; stack=stack->next) {
		if (stack->port == port) return stack->ptp;
	}
	return -1;
}

int misdn_lib_get_maxchans(int port) 
{
	struct misdn_stack *stack=get_misdn_stack();
	for ( ; stack; stack=stack->next) {
		if (stack->port == port) {
			if (stack->pri) 
				return 30;
			else
				return 2;
		}
	}
	return -1;
}


struct misdn_stack* get_stack_by_bc(struct misdn_bchannel *bc)
{
	struct misdn_stack *stack=get_misdn_stack();

	if (!bc) return NULL;
	
	for ( ; stack; stack=stack->next) {
		int i;
		for (i=0; i <stack->b_num; i++) {
			if ( bc->port == stack->port) return stack;
		}
	}

	return NULL;
}


void get_show_stack_details(int port, char *buf)
{
	struct misdn_stack *stack=get_misdn_stack();
	
	for ( ; stack; stack=stack->next) {
		if (stack->port == port) break;
	}
	
	if (stack) {
		sprintf(buf, "* Stack Addr:%x Port %d Type %s Prot. %s L2Link %s L1Link:%s", stack->upper_id, stack->port, stack->nt?"NT":"TE", stack->ptp?"PTP":"PMP", stack->l2link?"UP":"DOWN", stack->l1link?"UP":"DOWN");

	} else {
		buf[0]=0;
	}
	
}


static int nt_err_cnt =0 ;

enum global_states {
	MISDN_INITIALIZING,
	MISDN_INITIALIZED
} ;

static enum global_states  global_state=MISDN_INITIALIZING;


#include <mISDNuser/net_l2.h>
#include <mISDNuser/tone.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <signal.h>

#include "isdn_lib.h"


struct misdn_lib {
	int midev;
	int midev_nt;

	pthread_t l1watcher_thread;
	pthread_t event_thread;
	pthread_t event_handler_thread;

	int l1watcher_timeout;
	
	void *user_data;

	msg_queue_t upqueue;
	msg_queue_t activatequeue; 
  
	sem_t new_msg;
  
	struct misdn_stack *stack_list;
} ;

#ifndef ECHOCAN_ON
#define ECHOCAN_ON 123
#define ECHOCAN_OFF 124
#endif

#define MISDN_DEBUG 0

void misdn_tx_jitter(struct misdn_bchannel *bc, int len);

struct misdn_bchannel *find_bc_by_l3id(struct misdn_stack *stack, unsigned long l3id);

int setup_bc(struct misdn_bchannel *bc);

int manager_isdn_handler(iframe_t *frm ,msg_t *msg);

int misdn_lib_port_restart(int port);

extern struct isdn_msg msgs_g[]; 

#define ISDN_PID_L3_B_USER 0x430000ff
#define ISDN_PID_L4_B_USER 0x440000ff

/* #define MISDN_IBUF_SIZE 1024 */
#define MISDN_IBUF_SIZE 512

/*  Fine Tuning of Inband  Signalling time */
#define TONE_ALERT_CNT 41 /*  1 Sec  */
#define TONE_ALERT_SILENCE_CNT 200 /*  4 Sec */

#define TONE_BUSY_CNT 20 /*  ? */
#define TONE_BUSY_SILENCE_CNT 48 /*  ? */

static int  entity;

static struct misdn_lib *glob_mgr;

unsigned char tone_425_flip[TONE_425_SIZE];
unsigned char tone_silence_flip[TONE_SILENCE_SIZE];

static void misdn_lib_isdn_l1watcher(void *arg);
static void misdn_lib_isdn_event_catcher(void *arg);
static int handle_event_nt(void *dat, void *arg);


void stack_holder_add(struct misdn_stack *stack, struct misdn_bchannel *holder);
void stack_holder_remove(struct misdn_stack *stack, struct misdn_bchannel *holder);
struct misdn_bchannel *stack_holder_find(struct misdn_stack *stack, unsigned long l3id);

/* from isdn_lib.h */
int init_bc(struct misdn_stack * stack,  struct misdn_bchannel *bc, int midev, int port, int bidx, char *msn, int firsttime);
struct misdn_stack* stack_init(int midev,  int port, int ptp);
void stack_te_destroy(struct misdn_stack* stack);
	/* user iface */
int te_lib_init( void ) ; /* returns midev */
void te_lib_destroy(int midev) ;
struct misdn_bchannel *manager_find_bc_by_pid(int pid);
struct misdn_bchannel *manager_find_bc_holded(struct misdn_bchannel* bc);
void manager_ph_control_block(struct misdn_bchannel *bc, int c1, void *c2, int c2_len);
void manager_clean_bc(struct misdn_bchannel *bc );
void manager_bchannel_setup (struct misdn_bchannel *bc);
void manager_bchannel_cleanup (struct misdn_bchannel *bc);

int isdn_msg_get_index(struct isdn_msg msgs[], msg_t *frm, int nt);
enum event_e isdn_msg_get_event(struct isdn_msg msgs[], msg_t *frm, int nt);
int isdn_msg_parse_event(struct isdn_msg msgs[], msg_t *frm, struct misdn_bchannel *bc, int nt);
char * isdn_get_info(struct isdn_msg msgs[], enum event_e event, int nt);
msg_t * isdn_msg_build_event(struct isdn_msg msgs[], struct misdn_bchannel *bc, enum event_e event, int nt);
void ec_chunk( struct misdn_bchannel *bc, unsigned char *rxchunk, unsigned char *txchunk, int chunk_size);
	/* end */
int bchdev_echocancel_activate(struct misdn_bchannel* dev);
void bchdev_echocancel_deactivate(struct misdn_bchannel* dev);
/* end */


static char *bearer2str(int cap) {
	static char *bearers[]={
		"Speech",
		"Audio 3.1k",
		"Unres Digital",
		"Res Digital",
		"Unknown Bearer"
	};
	
	switch (cap) {
	case INFO_CAPABILITY_SPEECH:
		return bearers[0];
		break;
	case INFO_CAPABILITY_AUDIO_3_1K:
		return bearers[1];
		break;
	case INFO_CAPABILITY_DIGITAL_UNRESTRICTED:
		return bearers[2];
		break;
	case INFO_CAPABILITY_DIGITAL_RESTRICTED:
		return bearers[3];
		break;
	default:
		return bearers[4];
		break;
	}
}


static char flip_table[256];

void init_flip_bits(void)
{
	int i,k;
	
	for (i = 0 ; i < 256 ; i++) {
		unsigned char sample = 0 ;
		for (k = 0; k<8; k++) {
			if ( i & 1 << k ) sample |= 0x80 >>  k;
		}
		flip_table[i] = sample;
	}
}

unsigned char * flip_buf_bits ( unsigned char * buf , int len)
{
	int i;
	char * start = buf;
	
	for (i = 0 ; i < len; i++) {
		buf[i] = flip_table[buf[i]];
	}
	
	return start;
}




msg_t *create_l2msg(int prim, int dinfo, int size) /* NT only */
{
	int i = 0;
	msg_t *dmsg;
  
	while(i < 10)
	{
		dmsg = prep_l3data_msg(prim, dinfo, size, 256, NULL);
		if (dmsg)
			return(dmsg);
      
		if (!i)
			printf("cannot allocate memory, trying again...\n");
		i++;
		usleep(300000);
	}
	printf("cannot allocate memory, system overloaded.\n");
	exit(-1);
}



msg_t *create_l3msg(int prim, int mt, int dinfo, int size, int ntmode)
{
	int i = 0;
	msg_t *dmsg;
	Q931_info_t *qi;
	iframe_t *frm;
  
	if (!ntmode)
		size = sizeof(Q931_info_t)+2;
  
	while(i < 10) {
		if (ntmode) {
			dmsg = prep_l3data_msg(prim, dinfo, size, 256, NULL);
			if (dmsg) {
				return(dmsg);
			}
		} else {
			dmsg = alloc_msg(size+256+mISDN_HEADER_LEN+DEFAULT_HEADROOM);
			if (dmsg)
			{
				memset(msg_put(dmsg,size+mISDN_HEADER_LEN), 0, size+mISDN_HEADER_LEN);
				frm = (iframe_t *)dmsg->data;
				frm->prim = prim;
				frm->dinfo = dinfo;
				qi = (Q931_info_t *)(dmsg->data + mISDN_HEADER_LEN);
				qi->type = mt;
				return(dmsg);
			}
		}
    
		if (!i) printf("cannot allocate memory, trying again...\n");
		i++;
		usleep(300000);
	}
	printf("cannot allocate memory, system overloaded.\n");
	exit(-1);
}


int send_msg (int midev, struct misdn_bchannel *bc, msg_t *dmsg)
{
	iframe_t *frm;
	frm = (iframe_t *)dmsg->data;
	struct misdn_stack *stack=get_stack_by_bc(bc);

	if (!stack) {
		cb_log(0,bc->port,"send_msg: IEK!! no stack\n ");
		return -1;
	}
	
	frm->addr = (stack->upper_id | FLG_MSG_DOWN);

	
	frm->dinfo = bc->l3_id;
  
	frm->len = (dmsg->len) - mISDN_HEADER_LEN;
	
	mISDN_write(midev, dmsg->data, dmsg->len, TIMEOUT_1SEC);
  
	free_msg(dmsg);

	return 0;
}


static int mypid=1;


int misdn_cap_is_speech(int cap)
/** Poor mans version **/
{
	if ( (cap != INFO_CAPABILITY_DIGITAL_UNRESTRICTED) &&
	     (cap != INFO_CAPABILITY_DIGITAL_RESTRICTED) ) return 1;
	return 0;
}

int misdn_inband_avail(struct misdn_bchannel *bc)
{

	/*if ! early_bconnect we have never inband available*/
	if ( ! bc->early_bconnect ) return 0;
	
	switch (bc->progress_indicator) {
	case INFO_PI_INBAND_AVAILABLE:
	case INFO_PI_CALL_NOT_E2E_ISDN:
	case INFO_PI_CALLED_NOT_ISDN:
		return 1;
	default:
		return 0;
	}
	return 0;
}


void dump_chan_list(struct misdn_stack *stack)
{
	int i;

	for (i=0; i <stack->b_num; i++) {
		cb_log(6, stack->port, "Idx:%d stack->cchan:%d Chan:%d\n",i,stack->channels[i], i+1);
	}
}




static int find_free_chan_in_stack(struct misdn_stack *stack, struct misdn_bchannel *bc, int channel)
{
	int i;

	cb_log(1,stack->port,"find_free_chan: req_chan:%d\n",channel);

	if (channel < 0 || channel > MAX_BCHANS) {
		cb_log(4, stack->port, " !! out of bound call to find_free_chan_in_stack! (ch:%d)\n", channel);
		return 0;
	}
	
	channel--;
  
	for (i = 0; i < stack->b_num; i++) {
		if (i != 15 && (channel < 0 || i == channel)) { /* skip E1 Dchannel ;) and work with chan preselection */
			if (!stack->channels[i]) {
				cb_log (1, stack->port, " --> found chan%s: %d\n", channel>=0?" (preselected)":"", i+1);
				stack->channels[i] = 1;
				bc->channel=i+1;
				cb_event(EVENT_NEW_CHANNEL, bc, NULL);
				return i+1;
			}
		}
	}

	cb_log (4, stack->port, " !! NO FREE CHAN IN STACK\n");
	dump_chan_list(stack);
  
	return 0;
}

int empty_chan_in_stack(struct misdn_stack *stack, int channel)
{
	cb_log (1, stack?stack->port:0, "empty_chan_in_stack: %d\n",channel); 
	stack->channels[channel-1] = 0;
	dump_chan_list(stack);
	return 0;
}

char *bc_state2str(enum bchannel_state state) {
	int i;
	
	struct bchan_state_s {
		char *n;
		enum bchannel_state s;
	} states[] = {
		{"BCHAN_CLEANED", BCHAN_CLEANED },
		{"BCHAN_EMPTY", BCHAN_EMPTY},
		{"BCHAN_SETUP", BCHAN_SETUP},
		{"BCHAN_SETUPED", BCHAN_SETUPED},
		{"BCHAN_ACTIVE", BCHAN_ACTIVE},
		{"BCHAN_ACTIVATED", BCHAN_ACTIVATED},
		{"BCHAN_BRIDGE",  BCHAN_BRIDGE},
		{"BCHAN_BRIDGED", BCHAN_BRIDGED},
		{"BCHAN_RELEASE", BCHAN_RELEASE},
		{"BCHAN_RELEASED", BCHAN_RELEASED},
		{"BCHAN_CLEAN", BCHAN_CLEAN},
		{"BCHAN_CLEAN_REQUEST", BCHAN_CLEAN_REQUEST},
		{"BCHAN_ERROR", BCHAN_ERROR}
	};
	
	for (i=0; i< sizeof(states)/sizeof(struct bchan_state_s); i++)
		if ( states[i].s == state)
			return states[i].n;

	return "UNKNOWN";
}

void bc_state_change(struct misdn_bchannel *bc, enum bchannel_state state)
{
	cb_log(3,bc->port,"BC_STATE_CHANGE: from:%s to:%s\n",
	       bc_state2str(bc->bc_state),
	       bc_state2str(state) );
	
	switch (state) {
		case BCHAN_ACTIVATED:
			if (bc->next_bc_state ==  BCHAN_BRIDGED) {
				misdn_join_conf(bc, bc->conf_id);
				bc->next_bc_state = BCHAN_EMPTY;
				return;
			}
		default:
			bc->bc_state=state;
			break;
	}
}

void bc_next_state_change(struct misdn_bchannel *bc, enum bchannel_state state)
{
	cb_log(3,bc->port,"BC_NEXT_STATE_CHANGE: from:%s to:%s\n",
	       bc_state2str(bc->next_bc_state),
	       bc_state2str(state) );

	bc->next_bc_state=state;
}


void empty_bc(struct misdn_bchannel *bc)
{
	bc->bframe_len=0;
	
	bc->channel = 0;
	bc->in_use = 0;

	bc->sending_complete = 0;

	bc->restart_channel=0;
	
	bc->conf_id = 0;

	bc->need_more_infos = 0;
	
	bc->send_dtmf=0;
	bc->nodsp=0;
	bc->nojitter=0;

	bc->time_usec=0;
	
	bc->rxgain=0;
	bc->txgain=0;

	bc->crypt=0;
	bc->curptx=0; bc->curprx=0;
	
	bc->crypt_key[0] = 0;
	
	bc->generate_tone=0;
	bc->tone_cnt=0;
  
	bc->dnumplan=NUMPLAN_UNKNOWN;
	bc->onumplan=NUMPLAN_UNKNOWN;
	bc->rnumplan=NUMPLAN_UNKNOWN;
	bc->cpnnumplan=NUMPLAN_UNKNOWN;
	

	bc->active = 0;

	bc->early_bconnect = 1;
	
	bc->ec_enable = 0;
	bc->ec_deftaps = 128;
	bc->ec_whenbridged = 0;
	bc->ec_training = 1;
	
	
	bc->orig=0;
  
	bc->cause=16;
	bc->out_cause=16;
	bc->pres=0 ; /* screened */
	
	bc->evq=EVENT_NOTHING;

	bc->progress_coding=0;
	bc->progress_location=0;
	bc->progress_indicator=0;
	
/** Set Default Bearer Caps **/
	bc->capability=INFO_CAPABILITY_SPEECH;
	bc->law=INFO_CODEC_ALAW;
	bc->mode=0;
	bc->rate=0x10;
	bc->user1=0;
	bc->urate=0;
	
	bc->hdlc=0;
	
	
	bc->info_dad[0] = 0;
	bc->display[0] = 0;
	bc->infos_pending[0] = 0;
	bc->cad[0] = 0;
	bc->oad[0] = 0;
	bc->dad[0] = 0;
	bc->rad[0] = 0;
	bc->orig_dad[0] = 0;
	
	bc->fac_type=FACILITY_NONE;
	
	bc->te_choose_channel = 0;

	bc->holded_bc=NULL;
}


int clean_up_bc(struct misdn_bchannel *bc)
{
	int ret=0;
	unsigned char buff[32];
	struct misdn_stack * stack;

	cb_log(3, bc?bc->port:0, "$$$ CLEANUP CALLED pid:%d\n", bc?bc->pid:-1);
	
	if (!bc  ) return -1;
	stack=get_stack_by_bc(bc);
	
	if (!stack) return -1;
	
	switch (bc->bc_state ) {
	case BCHAN_CLEANED:
		cb_log(5, stack->port, "$$$ Already cleaned up bc with stid :%x\n", bc->b_stid);
		return -1;
		
	default:
		break;
	}
	
	cb_log(2, stack->port, "$$$ Cleaning up bc with stid :%x pid:%d\n", bc->b_stid, bc->pid);
	
	manager_bchannel_deactivate(bc);

	if ( misdn_cap_is_speech(bc->capability) && bc->ec_enable) {
		manager_ec_disable(bc);
	}

	mISDN_write_frame(stack->midev, buff, bc->layer_id|FLG_MSG_TARGET|FLG_MSG_DOWN, MGR_DELLAYER | REQUEST, 0, 0, NULL, TIMEOUT_1SEC);
	
	/*mISDN_clear_stack(stack->midev, bc->b_stid);*/

	

	bc->b_stid = 0;
	bc_state_change(bc, BCHAN_CLEANED);
	
	return ret;
}



void clear_l3(struct misdn_stack *stack)
{
	int i;
	for (i=0; i<stack->b_num; i++) {
		if (global_state == MISDN_INITIALIZED)  {
			cb_event(EVENT_CLEANUP, &stack->bc[i], glob_mgr->user_data);
			empty_chan_in_stack(stack,i+1);
			empty_bc(&stack->bc[i]);
			clean_up_bc(&stack->bc[i]);
		}
		
	} 
}

int set_chan_in_stack(struct misdn_stack *stack, int channel)
{

	cb_log(1,stack->port,"set_chan_in_stack: %d\n",channel);
	if (channel >=1 ) {
		stack->channels[channel-1] = 1;
	} else {
		cb_log(-1,stack->port,"couldn't set channel %d in\n", channel );
	}
  
	return 0;
}

int chan_in_stack_free(struct misdn_stack *stack, int channel)
{
	if (stack->channels[channel-1])
		return 0;
  
	return 1;
}



static int newteid=0;

#define MAXPROCS 0x100

int misdn_lib_get_l1_down(struct misdn_stack *stack)
{
	/* Pull Up L1 */ 
	iframe_t act;
	act.prim = PH_DEACTIVATE | REQUEST; 
	act.addr = (stack->upper_id | FLG_MSG_DOWN)  ;

	
	act.dinfo = 0;
	act.len = 0;

	return mISDN_write(stack->midev, &act, mISDN_HEADER_LEN+act.len, TIMEOUT_1SEC);


}


int misdn_lib_get_l2_down(struct misdn_stack *stack)
{
	
	if (stack->ptp && (stack->nt) ) {
		msg_t *dmsg;
		/* L2 */
		dmsg = create_l2msg(DL_RELEASE| REQUEST, 0, 0);
		
		if (stack->nst.manager_l3(&stack->nst, dmsg))
			free_msg(dmsg);
		
	} else {
		iframe_t act;
		
		act.prim = DL_RELEASE| REQUEST;
		act.addr = (stack->upper_id |FLG_MSG_DOWN)  ;
		
		act.dinfo = 0;
		act.len = 0;
		return mISDN_write(stack->midev, &act, mISDN_HEADER_LEN+act.len, TIMEOUT_1SEC);
	}
	
	return 0;
}


int misdn_lib_get_l1_up(struct misdn_stack *stack)
{
	/* Pull Up L1 */ 
	iframe_t act;
	act.prim = PH_ACTIVATE | REQUEST; 
	act.addr = (stack->upper_id | FLG_MSG_DOWN)  ;

	
	act.dinfo = 0;
	act.len = 0;

	return mISDN_write(stack->midev, &act, mISDN_HEADER_LEN+act.len, TIMEOUT_1SEC);

}

int misdn_lib_get_l2_up(struct misdn_stack *stack)
{
	
	if (stack->ptp && (stack->nt) ) {
		msg_t *dmsg;
		/* L2 */
		dmsg = create_l2msg(DL_ESTABLISH | REQUEST, 0, 0);
		
		if (stack->nst.manager_l3(&stack->nst, dmsg))
			free_msg(dmsg);
		
	} else {
		iframe_t act;
		
		act.prim = DL_ESTABLISH | REQUEST;
		act.addr = (stack->upper_id |FLG_MSG_DOWN)  ;
		
		act.dinfo = 0;
		act.len = 0;
		return mISDN_write(stack->midev, &act, mISDN_HEADER_LEN+act.len, TIMEOUT_1SEC);
	}
	
	return 0;
}

int misdn_lib_get_l2_te_ptp_up(struct misdn_stack *stack)
{
	iframe_t act;
		
	act.prim = DL_ESTABLISH | REQUEST;
	act.addr = (stack->upper_id  & ~LAYER_ID_MASK) | 3 | FLG_MSG_DOWN;
		
	act.dinfo = 0;
	act.len = 0;
	return mISDN_write(stack->midev, &act, mISDN_HEADER_LEN+act.len, TIMEOUT_1SEC);
	return 0;
}

int misdn_lib_get_l2_status(struct misdn_stack *stack)
{
	iframe_t act;
	
	act.prim = DL_ESTABLISH | REQUEST;

	act.addr = (stack->upper_id | FLG_MSG_DOWN)  ;

	act.dinfo = 0;
	act.len = 0;
	return mISDN_write(stack->midev, &act, mISDN_HEADER_LEN+act.len, TIMEOUT_1SEC);
}

int misdn_lib_get_short_status(struct misdn_stack *stack)
{
	iframe_t act;
	
	
	act.prim = MGR_SHORTSTATUS | REQUEST; 
	
	act.addr = (stack->upper_id | MSG_BROADCAST)  ;

	act.dinfo = SSTATUS_BROADCAST_BIT | SSTATUS_ALL;
	
	act.len = 0;
	return mISDN_write(stack->midev, &act, mISDN_HEADER_LEN+act.len, TIMEOUT_1SEC);
}



static int create_process (int midev, struct misdn_bchannel *bc) {
	iframe_t ncr;
	int l3_id;
	int i;
	struct misdn_stack *stack=get_stack_by_bc(bc);
	int free_chan;
  
	if (stack->nt) {
		free_chan = find_free_chan_in_stack(stack, bc, bc->channel_preselected?bc->channel:0);
		if (!free_chan) return -1;
		/*bc->channel=free_chan;*/
		
		cb_log(4,stack->port, " -->  found channel: %d\n",free_chan);
    
		for (i=0; i <= MAXPROCS; i++)
			if (stack->procids[i]==0) break;
    
		if (i== MAXPROCS) {
			cb_log(-1, stack->port, "Couldnt Create New ProcId.\n");
			return -1;
		}
		stack->procids[i]=1;

		l3_id = 0xff00 | i;
    
		ncr.prim = CC_NEW_CR | REQUEST; 

		ncr.addr = (stack->upper_id | FLG_MSG_DOWN)  ;

		ncr.dinfo = l3_id;
		ncr.len = 0;

		bc->l3_id = l3_id;
		if (mypid>5000) mypid=1;
		bc->pid=mypid++;
      
		cb_log(3, stack->port, " --> new_l3id %x\n",l3_id);
    
	} else { 
		if (stack->ptp || bc->te_choose_channel) {
			/* we know exactly which channels are in use */
			free_chan = find_free_chan_in_stack(stack, bc, bc->channel_preselected?bc->channel:0);
			if (!free_chan) return -1;
			/*bc->channel=free_chan;*/
			cb_log(2,stack->port, " -->  found channel: %d\n",free_chan);
		} else {
			/* other phones could have made a call also on this port (ptmp) */
			bc->channel=0xff;
		}
    
    
		/* if we are in te-mode, we need to create a process first */
		if (newteid++ > 0xffff)
			newteid = 0x0001;
    
		l3_id = (entity<<16) | newteid;
		/* preparing message */
		ncr.prim = CC_NEW_CR | REQUEST; 

		ncr.addr = (stack->upper_id | FLG_MSG_DOWN)  ;

		ncr.dinfo =l3_id;
		ncr.len = 0;
		/* send message */

		bc->l3_id = l3_id;
		if (mypid>5000) mypid=1;
		bc->pid=mypid++;
    
		cb_log(3, stack->port, "--> new_l3id %x\n",l3_id);
    
		mISDN_write(midev, &ncr, mISDN_HEADER_LEN+ncr.len, TIMEOUT_1SEC);
	}
  
	return l3_id;
}


void misdn_lib_setup_bc(struct misdn_bchannel *bc)
{
	setup_bc(bc);
}


int setup_bc(struct misdn_bchannel *bc)
{
	unsigned char buff[1025];
  
	mISDN_pid_t pid;
	int ret;
	

	struct misdn_stack *stack=get_stack_by_bc(bc);

	if (!stack) {
		cb_log(-1, bc->port, "setup_bc: NO STACK FOUND!!\n");
		return -1;
	}
	
	int midev=stack->midev;
	int channel=bc->channel-1-(bc->channel>16);
	int b_stid=stack->b_stids[channel>=0?channel:0];


	switch (bc->bc_state) {
		case BCHAN_CLEANED:
			break;
		default:
			cb_log(4, stack->port, "$$$ bc already upsetted stid :%x (state:%s)\n", b_stid, bc_state2str(bc->bc_state) );
			return -1;
	}
	
	cb_log(5, stack->port, "$$$ Setting up bc with stid :%x\n", b_stid);
	
	if (b_stid <= 0) {
		cb_log(-1, stack->port," -- Stid <=0 at the moment in channel:%d\n",channel);
		
		bc_state_change(bc,BCHAN_ERROR);
		return 1;
	}
	
	
	bc->b_stid = b_stid;
	
	{
		layer_info_t li;
		memset(&li, 0, sizeof(li));
    
		li.object_id = -1;
		li.extentions = 0;
		
		li.st = bc->b_stid; /*  given idx */


#define MISDN_DSP
#ifndef MISDN_DSP
		bc->nodsp=1;
#endif
		if ( bc->hdlc || bc->nodsp) {
			cb_log(4, stack->port,"setup_bc: without dsp\n");
			{ 
				int l = sizeof(li.name);
				strncpy(li.name, "B L3", l);
				li.name[l-1] = 0;
			}
			li.pid.layermask = ISDN_LAYER((3));
			li.pid.protocol[3] = ISDN_PID_L3_B_USER;
			
			bc->layer=3;
		} else {
			cb_log(4, stack->port,"setup_bc: with dsp\n");
			{ 
				int l = sizeof(li.name);
				strncpy(li.name, "B L4", l);
				li.name[l-1] = 0;
			}
			li.pid.layermask = ISDN_LAYER((4));
			li.pid.protocol[4] = ISDN_PID_L4_B_USER
;
			bc->layer=4;
			
		}  
		
		ret = mISDN_new_layer(midev, &li);
		if (ret ) {
			cb_log(-1, stack->port,"New Layer Err: %d %s\n",ret,strerror(errno));

			bc_state_change(bc,BCHAN_ERROR);
			return(-EINVAL);
		}
		
		bc->layer_id = li.id;
	}
	
	memset(&pid, 0, sizeof(pid));
	
	
	
	cb_log(4, stack->port," --> Channel is %d\n", bc->channel);
	
	if (bc->nodsp) {
		cb_log(2, stack->port," --> TRANSPARENT Mode (no DSP, no HDLC)\n");
		pid.protocol[1] = ISDN_PID_L1_B_64TRANS;
		pid.protocol[2] = ISDN_PID_L2_B_TRANS;
		pid.protocol[3] = ISDN_PID_L3_B_USER;
		pid.layermask = ISDN_LAYER((1)) | ISDN_LAYER((2)) | ISDN_LAYER((3));
		
	} else if ( bc->hdlc ) {
		cb_log(2, stack->port," --> HDLC Mode\n");
#ifdef ACK_HDLC
		bc->ack_hdlc=(sem_t*)malloc(sizeof(sem_t));
		if ( sem_init((sem_t*)bc->ack_hdlc, 1, 0)<0 )
			sem_init((sem_t*)bc->ack_hdlc, 0, 0);
#endif
		
		pid.protocol[1] = ISDN_PID_L1_B_64HDLC ;
		pid.protocol[2] = ISDN_PID_L2_B_TRANS  ;
		pid.protocol[3] = ISDN_PID_L3_B_USER;
		pid.layermask = ISDN_LAYER((1)) | ISDN_LAYER((2)) | ISDN_LAYER((3)) ;
	} else {
		cb_log(2, stack->port," --> TRANSPARENT Mode\n");
		pid.protocol[1] = ISDN_PID_L1_B_64TRANS;
		pid.protocol[2] = ISDN_PID_L2_B_TRANS;
		pid.protocol[3] = ISDN_PID_L3_B_DSP;
		pid.protocol[4] = ISDN_PID_L4_B_USER;
		pid.layermask = ISDN_LAYER((1)) | ISDN_LAYER((2)) | ISDN_LAYER((3)) | ISDN_LAYER((4));
		
	} 

	ret = mISDN_set_stack(midev, bc->b_stid, &pid);

	if (ret){
		cb_log(-1, stack->port,"$$$ Set Stack Err: %d %s\n",ret,strerror(errno));
		
		mISDN_write_frame(midev, buff, bc->layer_id, MGR_DELLAYER | REQUEST, 0, 0, NULL, TIMEOUT_1SEC);
		
		bc_state_change(bc,BCHAN_ERROR);
		return(-EINVAL);
	}


	ret = mISDN_get_setstack_ind(midev, bc->layer_id);

	if (ret) {
		cb_log(-1, stack->port,"$$$ Set StackIND Err: %d %s\n",ret,strerror(errno));
		mISDN_write_frame(midev, buff, bc->layer_id, MGR_DELLAYER | REQUEST, 0, 0, NULL, TIMEOUT_1SEC);
		
		bc_state_change(bc,BCHAN_ERROR);
		return(-EINVAL);
	}

	ret = mISDN_get_layerid(midev, bc->b_stid, bc->layer) ;

	bc->addr = ret>0? ret : 0;

	if (!bc->addr) {
		cb_log(-1, stack->port,"$$$ Get Layerid Err: %d %s\n",ret,strerror(errno));
		mISDN_write_frame(midev, buff, bc->layer_id, MGR_DELLAYER | REQUEST, 0, 0, NULL, TIMEOUT_1SEC);
		
		bc_state_change(bc,BCHAN_ERROR);
	}

	manager_bchannel_activate(bc);
	
	bc_state_change(bc,BCHAN_ACTIVATED);

	return 0;
}



/** IFACE **/
int init_bc(struct misdn_stack *stack,  struct misdn_bchannel *bc, int midev, int port, int bidx,  char *msn, int firsttime)
{
	unsigned char buff[1025];
	iframe_t *frm = (iframe_t *)buff;
	int ret;
  
	if (!bc) return -1;
  
	cb_log(4, port, "Init.BC %d.\n",bidx);
	
	memset(bc, 0,sizeof(struct misdn_bchannel));
	
	if (msn) {
		int l = sizeof(bc->msn);
		strncpy(bc->msn,msn, l);
		bc->msn[l-1] = 0;
	}
	
	
	empty_bc(bc);
	bc_state_change(bc, BCHAN_CLEANED);
	
	bc->port=stack->port;
	bc->nt=stack->nt?1:0;
	
	{
		ibuffer_t* ibuf= init_ibuffer(MISDN_IBUF_SIZE);

		if (!ibuf) return -1;
		
		clear_ibuffer( ibuf);
		
		ibuf->rsem=malloc(sizeof(sem_t));
		
		bc->astbuf=ibuf;

		if (sem_init(ibuf->rsem,1,0)<0)
			sem_init(ibuf->rsem,0,0);
		
	}
	
	
	
	
	{
		stack_info_t *stinf;
		ret = mISDN_get_stack_info(midev, stack->port, buff, sizeof(buff));
		if (ret < 0) {
			cb_log(-1, port, "%s: Cannot get stack info for this port. (ret=%d)\n", __FUNCTION__, ret);
			return -1;
		}
    
		stinf = (stack_info_t *)&frm->data.p;
    
		cb_log(4, port, " --> Child %x\n",stinf->child[bidx]);
	}
  
	return 0;
}



struct misdn_stack* stack_init( int midev, int port, int ptp )
{
	int ret;
	unsigned char buff[1025];
	iframe_t *frm = (iframe_t *)buff;
	stack_info_t *stinf;
	int i; 
	layer_info_t li;

	struct misdn_stack *stack = malloc(sizeof(struct misdn_stack));
	if (!stack ) return NULL;


	cb_log(4, port, "Init. Stack.\n");
  
	memset(stack,0,sizeof(struct misdn_stack));
  
	for (i=0; i<MAX_BCHANS + 1; i++ ) stack->channels[i]=0;
	
	stack->port=port;
	stack->midev=midev;
	stack->ptp=ptp;
  
	stack->holding=NULL;
	stack->pri=0;
  
	msg_queue_init(&stack->downqueue);
	msg_queue_init(&stack->upqueue);
  
	/* query port's requirements */
	ret = mISDN_get_stack_info(midev, port, buff, sizeof(buff));
	if (ret < 0) {
		cb_log(-1, port, "%s: Cannot get stack info for this port. (ret=%d)\n", __FUNCTION__, ret);
		return(NULL);
	}
  
	stinf = (stack_info_t *)&frm->data.p;

	stack->d_stid = stinf->id;
	stack->b_num = stinf->childcnt;

	for (i=0; i<stinf->childcnt; i++)
		stack->b_stids[i] = stinf->child[i];
  
	switch(stinf->pid.protocol[0] & ~ISDN_PID_FEATURE_MASK) {
	case ISDN_PID_L0_TE_S0:
		stack->nt=0;
		break;
	case ISDN_PID_L0_NT_S0:
		cb_log(4, port, "NT Stack\n");

		stack->nt=1;
		break;

	case ISDN_PID_L0_TE_U:
		break;
	case ISDN_PID_L0_NT_U:
		break;
	case ISDN_PID_L0_TE_UP2:
		break;
	case ISDN_PID_L0_NT_UP2:
		break;
	case ISDN_PID_L0_TE_E1:
		cb_log(4, port, "TE S2M Stack\n");
		stack->nt=0;
		stack->pri=1;
		break;
	case ISDN_PID_L0_NT_E1:
		cb_log(4, port, "TE S2M Stack\n");
		stack->nt=1;
		stack->pri=1;
		
		break;
	default:
		cb_log(-1, port, "this is a unknown port type 0x%08x\n", stinf->pid.protocol[0]);

	}

	if (!stack->nt) {
		if (stinf->pid.protocol[2] & ISDN_PID_L2_DF_PTP ) { 
			stack->ptp = 1;
		} else {
			stack->ptp = 0;
		}
	}
	
	{
		int ret;
		int nt=stack->nt;

		cb_log(4, port, "Init. Stack.\n");
		
		memset(&li, 0, sizeof(li));
		{
			int l = sizeof(li.name);
			strncpy(li.name,nt?"net l2":"user l4", l);
			li.name[l-1] = 0;
		}
		li.object_id = -1;
		li.extentions = 0;
		li.pid.protocol[nt?2:4] = nt?ISDN_PID_L2_LAPD_NET:ISDN_PID_L4_CAPI20;
		li.pid.layermask = ISDN_LAYER((nt?2:4));
		li.st = stack->d_stid;
		
		
		ret = mISDN_new_layer(midev, &li);
		if (ret) {
			cb_log(-1, port, "%s: Cannot add layer %d to this port.\n", __FUNCTION__, nt?2:4);
			return(NULL);
		}
		
		
		stack->upper_id = li.id;
		ret = mISDN_register_layer(midev, stack->d_stid, stack->upper_id);
		if (ret)
		{
			cb_log(-1,port,"Cannot register layer %d of this port.\n", nt?2:4);
			return(NULL);
		}
		
		stack->lower_id = mISDN_get_layerid(midev, stack->d_stid, nt?1:3); 
		if (stack->lower_id < 0) {
			cb_log(-1, port, "%s: Cannot get layer(%d) id of this port.\n", __FUNCTION__, nt?1:3);
			return(NULL);
		}
		
		stack->upper_id = mISDN_get_layerid(midev, stack->d_stid, nt?2:4);
		if (stack->upper_id < 0) {
			cb_log(-1, port, "%s: Cannot get layer(%d) id of this port.\n", __FUNCTION__, 2);
			return(NULL);
		}
		
		cb_log(4, port, "NT Stacks upper_id %x\n",stack->upper_id);
		
		
		/* create nst (nt-mode only) */
		if (nt) {
			
			memset(&stack->nst, 0, sizeof(net_stack_t));
			memset(&stack->mgr, 0, sizeof(manager_t));
    
			stack->mgr.nst = &stack->nst;
			stack->nst.manager = &stack->mgr;
    
			stack->nst.l3_manager = handle_event_nt;
			stack->nst.device = midev;
			stack->nst.cardnr = port;
			stack->nst.d_stid = stack->d_stid;
    
			stack->nst.feature = FEATURE_NET_HOLD;
			if (stack->ptp)
				stack->nst.feature |= FEATURE_NET_PTP;
			if (stack->pri)
				stack->nst.feature |= FEATURE_NET_CRLEN2 | FEATURE_NET_EXTCID;
			
			stack->nst.l1_id = stack->lower_id;
			stack->nst.l2_id = stack->upper_id;
			
			msg_queue_init(&stack->nst.down_queue);
			
			Isdnl2Init(&stack->nst);
			Isdnl3Init(&stack->nst);
			
		} 
		
		if (!stack->nt) {
			/*assume L1 is up, we'll get DEACTIVATES soon, for non
			 * up L1s*/
			stack->l1link=0;
		}

		misdn_lib_get_short_status(stack);
		misdn_lib_get_l1_up(stack);
		misdn_lib_get_l2_up(stack);
		
	}

	cb_log(1,0,"stack_init: port:%d lowerId:%x  upperId:%x\n",stack->port,stack->lower_id, stack->upper_id);
	
	return stack;
}


void stack_te_destroy(struct misdn_stack* stack)
{
	char buf[1024];
	if (!stack) return;
  
	if (stack->lower_id) 
		mISDN_write_frame(stack->midev, buf, stack->lower_id, MGR_DELLAYER | REQUEST, 0, 0, NULL, TIMEOUT_1SEC);

	if (stack->upper_id) 
		mISDN_write_frame(stack->midev, buf, stack->upper_id, MGR_DELLAYER | REQUEST, 0, 0, NULL, TIMEOUT_1SEC);
}


struct misdn_stack * find_stack_by_addr(int  addr)
{
	struct misdn_stack *stack;
	
	for (stack=glob_mgr->stack_list;
	     stack;
	     stack=stack->next) {
		if ( (stack->upper_id&STACK_ID_MASK) == (addr&STACK_ID_MASK)) return stack;

	}
  
	return NULL;
}


struct misdn_stack * find_stack_by_port(int port)
{
	struct misdn_stack *stack;
  
	for (stack=glob_mgr->stack_list;
	     stack;
	     stack=stack->next) 
		if (stack->port == port) return stack;
  
	return NULL;
}

struct misdn_stack * find_stack_by_mgr(manager_t* mgr_nt)
{
	struct misdn_stack *stack;
  
	for (stack=glob_mgr->stack_list;
	     stack;
	     stack=stack->next) 
		if ( &stack->mgr == mgr_nt) return stack;
  
	return NULL;
}

struct misdn_bchannel *find_bc_by_masked_l3id(struct misdn_stack *stack, unsigned long l3id, unsigned long mask)
{
	int i;
	for (i=0; i<stack->b_num; i++) {
		if ( (stack->bc[i].l3_id & mask)  ==  (l3id & mask)) return &stack->bc[i] ;
	}
	return stack_holder_find(stack,l3id);
}


struct misdn_bchannel *find_bc_by_l3id(struct misdn_stack *stack, unsigned long l3id)
{
	int i;
	for (i=0; i<stack->b_num; i++) {
		if (stack->bc[i].l3_id == l3id) return &stack->bc[i] ;
	}
	return stack_holder_find(stack,l3id);
}

struct misdn_bchannel *find_bc_holded(struct misdn_stack *stack)
{
	int i;
	for (i=0; i<stack->b_num; i++) {
		if (stack->bc[i].holded ) return &stack->bc[i] ;
	}
	return NULL;
}


struct misdn_bchannel *find_bc_by_addr(unsigned long addr)
{
	struct misdn_stack* stack;
	int i;

	
	for (stack=glob_mgr->stack_list;
	     stack;
	     stack=stack->next) {
		
		for (i=0; i< stack->b_num; i++) {

			if ( (stack->bc[i].addr&STACK_ID_MASK)==(addr&STACK_ID_MASK) ||  stack->bc[i].layer_id== addr ) {
				return &stack->bc[i];
			}
		}
		
	}

	
	return NULL;
}


struct misdn_bchannel *find_bc_by_channel(int port, int channel)
{
	struct misdn_stack* stack=find_stack_by_port(port);
	int i;

	if (!stack) return NULL;	
	
	for (i=0; i< stack->b_num; i++) {
		if ( stack->bc[i].channel== channel ) {
			return &stack->bc[i];
		}
	}
		
	return NULL;
}





int handle_event ( struct misdn_bchannel *bc, enum event_e event, iframe_t *frm)
{
	struct misdn_stack *stack=get_stack_by_bc(bc);
	
	if (!stack->nt) {
		
		switch (event) {

		case EVENT_CONNECT_ACKNOWLEDGE:
#if 0
			if ( !misdn_cap_is_speech(bc->capability)) {
				int ret=setup_bc(bc);
				if (ret == -EINVAL){
					cb_log(-1,bc->port,"send_event: setup_bc failed\n");
				}
			}
#endif	
			break;
		case EVENT_CONNECT:

			if ( *bc->crypt_key ) {
				cb_log(4, stack->port, "ENABLING BLOWFISH channel:%d oad%d:%s dad%d:%s\n", bc->channel, bc->onumplan,bc->oad, bc->dnumplan,bc->dad);
				manager_ph_control_block(bc,  BF_ENABLE_KEY, bc->crypt_key, strlen(bc->crypt_key) );
			}
		case EVENT_ALERTING:
		case EVENT_PROGRESS:
		case EVENT_PROCEEDING:
		case EVENT_SETUP_ACKNOWLEDGE:

		setup_bc(bc);
		
		case EVENT_SETUP:
			
		{
			if (bc->channel == 0xff) {
				bc->channel=find_free_chan_in_stack(stack, bc,  0);
				if (!bc->channel) {
					cb_log(-1, stack->port, "Any Channel Requested, but we have no more!!\n");
					break;
				}
			}  

			if (bc->channel >0 && bc->channel<255) {
				set_chan_in_stack(stack ,bc->channel);
			}

#if 0
			int ret=setup_bc(bc);
			if (ret == -EINVAL){
				cb_log(-1,bc->port,"handle_event: setup_bc failed\n");
				misdn_lib_send_event(bc,EVENT_RELEASE_COMPLETE);
			}
#endif
		}
		break;

		case EVENT_RELEASE_COMPLETE:
		case EVENT_RELEASE:
			empty_chan_in_stack(stack,bc->channel);
			empty_bc(bc);
			clean_up_bc(bc);
			break;
		default:
			break;
		}
	} else {    /** NT MODE **/
		
	}
	return 0;
}

int handle_new_process(struct misdn_stack *stack, iframe_t *frm)
{
  
	struct misdn_bchannel* bc=misdn_lib_get_free_bc(stack->port, 0);
	
	
	if (!bc) {
		cb_log(-1, stack->port, " --> !! lib: No free channel!\n");
		return -1;
	}
  
	cb_log(7, stack->port, " --> new_process: New L3Id: %x\n",frm->dinfo);
	bc->l3_id=frm->dinfo;
	
	if (mypid>5000) mypid=1;
	bc->pid=mypid++;
	return 0;
}

int handle_cr ( struct misdn_stack *stack, iframe_t *frm)
{
	if (!stack) return -1;
  
	switch (frm->prim) {
	case CC_NEW_CR|INDICATION:
		cb_log(7, stack->port, " --> lib: NEW_CR Ind with l3id:%x on this port.\n",frm->dinfo);
		if (handle_new_process(stack, frm) <0) 
			return -1;
		return 1;
	case CC_NEW_CR|CONFIRM:
		return 1;
	case CC_NEW_CR|REQUEST:
		return 1;
	case CC_RELEASE_CR|REQUEST:
		return 1;
	case CC_RELEASE_CR|CONFIRM:
		break;
	case CC_RELEASE_CR|INDICATION:
		cb_log(4, stack->port, " --> lib: RELEASE_CR Ind with l3id:%x\n",frm->dinfo);
		{
			struct misdn_bchannel *bc=find_bc_by_l3id(stack, frm->dinfo);
			struct misdn_bchannel dummybc;
      
			if (!bc) {
				cb_log(4, stack->port, " --> Didn't found BC so temporarly creating dummy BC (l3id:%x) on this port.\n", frm->dinfo);
				memset (&dummybc,0,sizeof(dummybc));
				dummybc.port=stack->port;
				dummybc.l3_id=frm->dinfo;
				bc=&dummybc; 
			}
      
			if (bc) {
				cb_log(4, stack->port, " --> lib: CLEANING UP l3id: %x\n",frm->dinfo);
				empty_chan_in_stack(stack,bc->channel);
				empty_bc(bc);
				
				clean_up_bc(bc);
				
				dump_chan_list(stack);
				/*bc->pid = 0;*/
				cb_event(EVENT_CLEANUP, bc, glob_mgr->user_data);
				if (bc->stack_holder) {
					cb_log(4,stack->port, "REMOVEING Holder\n");
					stack_holder_remove( stack, bc);
					free(bc);
				}
			}
			else {
				if (stack->nt) 
					cb_log(4, stack->port, "BC with dinfo: %x  not found.. (prim was %x and addr %x)\n",frm->dinfo, frm->prim, frm->addr);
			}
      
			return 1;
		}
		break;
	}
  
	return 0;
}


/*Emptys bc if it's reserved (no SETUP out yet)*/
void misdn_lib_release(struct misdn_bchannel *bc)
{
	struct misdn_stack *stack=get_stack_by_bc(bc);

	if (!stack) {
		cb_log(1,0,"misdn_release: No Stack found\n");
		return;
	}
	
	if (bc->channel>=0) {
		empty_chan_in_stack(stack,bc->channel);
		empty_bc(bc);
	}
	clean_up_bc(bc);
}




int misdn_lib_get_port_up (int port) 
{ /* Pull Up L1 */ 
	struct misdn_stack *stack;
	
	for (stack=glob_mgr->stack_list;
	     stack;
	     stack=stack->next) {
		
		if (stack->port == port) {

			if (!stack->l1link)
				misdn_lib_get_l1_up(stack);
			if (!stack->l2link)
				misdn_lib_get_l2_up(stack);
			
			return 0;
		}
	}
	return 0;
}


int misdn_lib_get_port_down (int port) 
{ /* Pull Down L1 */ 
	struct misdn_stack *stack;
	for (stack=glob_mgr->stack_list;
	     stack;
	     stack=stack->next) {
		if (stack->port == port) {
				if (stack->l2link)
					misdn_lib_get_l2_down(stack);
				misdn_lib_get_l1_down(stack);
			return 0;
		}
	}
	return 0;
}

int misdn_lib_send_facility(struct misdn_bchannel *bc, enum facility_type fac, void *data)
{
	switch (fac) {
	case FACILITY_CALLDEFLECT:
		strcpy(bc->out_fac.calldeflect_nr,(char*)data);
		break;
	default:
		cb_log(1,bc?bc->port:0,"We don't handle this facility yet: %d\n",fac);
		return 0;
	}
	
	bc->out_fac_type=fac;
	
	misdn_lib_send_event(bc,EVENT_FACILITY);
	return 0;
}


int misdn_lib_port_up(int port, int check)
{
	struct misdn_stack *stack;


	for (stack=glob_mgr->stack_list;
	     stack;
	     stack=stack->next) {
		
		if ( !stack->ptp && !check) return 1;
		
		if (stack->port == port) {
			if (stack->l1link)
				return 1;
			else {
				cb_log(-1,port, "Port down [%s]\n",
				       stack->ptp?"PP":"PMP");
				return 0;
			}
		}
	}
  
	return -1;
}


int
handle_event_nt(void *dat, void *arg)
{
	manager_t *mgr = (manager_t *)dat;
	msg_t *msg = (msg_t *)arg;
	mISDNuser_head_t *hh;
	int reject=0;

	struct misdn_stack *stack=find_stack_by_mgr(mgr);
	int port;

	if (!msg || !mgr)
		return(-EINVAL);

	hh=(mISDNuser_head_t*)msg->data;
	port=stack->port;
	
	cb_log(4, stack->port, " --> lib: prim %x dinfo %x\n",hh->prim, hh->dinfo);
	{
		switch(hh->prim){
		case CC_RETRIEVE|INDICATION:
		{
			iframe_t frm; /* fake te frm to add callref to global callreflist */
			frm.dinfo = hh->dinfo;

			frm.addr=stack->upper_id | FLG_MSG_DOWN;

			frm.prim = CC_NEW_CR|INDICATION;
			
			if (handle_cr( stack, &frm)< 0) {
				msg_t *dmsg;
				cb_log(4, stack->port, "Patch from MEIDANIS:Sending RELEASE_COMPLETE %x (No free Chan for you..)\n", hh->dinfo);
				dmsg = create_l3msg(CC_RELEASE_COMPLETE | REQUEST,MT_RELEASE_COMPLETE, hh->dinfo,sizeof(RELEASE_COMPLETE_t), 1);
				stack->nst.manager_l3(&stack->nst, dmsg);
				free_msg(msg);
				return 0;
			}
			
			struct misdn_bchannel *bc=find_bc_by_l3id(stack, hh->dinfo);
			cb_event(EVENT_NEW_BC, bc, glob_mgr->user_data);
			struct misdn_bchannel *hold_bc=stack_holder_find(stack,bc->l3_id);
			if (hold_bc) {
				cb_log(4, stack->port, "REMOVEING Holder\n");
				stack_holder_remove(stack, hold_bc);
				free(hold_bc);
			}
			
		}
			
			break;
			
		case CC_SETUP|CONFIRM:
		{
			struct misdn_bchannel *bc=find_bc_by_l3id(stack, hh->dinfo);
			int l3id = *((int *)(((u_char *)msg->data)+ mISDNUSER_HEAD_SIZE));
			cb_log(4, stack->port, " --> lib: Event_ind:SETUP CONFIRM [NT] : new L3ID  is %x\n",l3id );
	
			if (!bc) { cb_log(4, stack->port, "Bc Not found (after SETUP CONFIRM)\n"); return 0; }
			cb_log (2,bc->port,"I IND :CC_SETUP|CONFIRM: old l3id:%x new l3id:%x\n", bc->l3_id, l3id);
			bc->l3_id=l3id;
			cb_event(EVENT_NEW_L3ID, bc, glob_mgr->user_data);
		}
		free_msg(msg);
		return 0;
      
		case CC_SETUP|INDICATION:
		{
			iframe_t frm; /* fake te frm to add callref to global callreflist */
			frm.dinfo = hh->dinfo;
			frm.addr=stack->upper_id;
			frm.prim = CC_NEW_CR|INDICATION;
			
			if (handle_cr(stack, &frm)< 0) {
				msg_t *dmsg;
				cb_log(4, stack->port, "Patch from MEIDANIS:Sending RELEASE_COMPLETE %x (No free Chan for you..)\n", hh->dinfo);
				dmsg = create_l3msg(CC_RELEASE_COMPLETE | REQUEST,MT_RELEASE_COMPLETE, hh->dinfo,sizeof(RELEASE_COMPLETE_t), 1);
				stack->nst.manager_l3(&stack->nst, dmsg);
				free_msg(msg);
				return 0;
			}
		}
		break;

		case CC_CONNECT_ACKNOWLEDGE|INDICATION:
#if 0
		{
			struct misdn_bchannel *bc=find_bc_by_l3id(stack, hh->dinfo);
			if (bc) {
				if ( !misdn_cap_is_speech(bc->capability)) {
					int ret=setup_bc(bc);
					if (ret == -EINVAL){
						cb_log(-1,bc->port,"send_event: setup_bc failed\n");
						
					}
				}
			}
		}
#endif
		break;
		
		case CC_ALERTING|INDICATION:
		case CC_PROCEEDING|INDICATION:
		case CC_SETUP_ACKNOWLEDGE|INDICATION:
			if(!stack->ptp) break;	
		case CC_CONNECT|INDICATION:
		{
#if 0
			struct misdn_bchannel *bc=find_bc_by_l3id(stack, hh->dinfo);
			
			if (!bc) {
				msg_t *dmsg;
				cb_log(-1, stack->port,"!!!! We didn't found our bc, dinfo:%x on this port.\n",hh->dinfo);
				
				cb_log(-1, stack->port, "Releaseing call %x (No free Chan for you..)\n", hh->dinfo);
				dmsg = create_l3msg(CC_RELEASE_COMPLETE | REQUEST,MT_RELEASE_COMPLETE, hh->dinfo,sizeof(RELEASE_COMPLETE_t), 1);
				stack->nst.manager_l3(&stack->nst, dmsg);
				free_msg(msg);
				return 0;
				
			}
			int ret=setup_bc(bc);
			if (ret == -EINVAL){
				cb_log(-1,bc->port,"handle_event_nt: setup_bc failed\n");
				misdn_lib_send_event(bc,EVENT_RELEASE_COMPLETE);
			}
#endif
		}
		break;
		case CC_DISCONNECT|INDICATION:
		{
			struct misdn_bchannel *bc=find_bc_by_l3id(stack, hh->dinfo);
			if (!bc) {
				bc=find_bc_by_masked_l3id(stack, hh->dinfo, 0xffff0000);
				if (bc) { 
					int myprocid=bc->l3_id&0x0000ffff;
					hh->dinfo=(hh->dinfo&0xffff0000)|myprocid;
					cb_log(3,stack->port,"Reject dinfo: %x cause:%d\n",hh->dinfo,bc->cause);
					reject=1;		
				}
			}
		}
		break;
		
		case CC_FACILITY|INDICATION:
		{
			struct misdn_bchannel *bc=find_bc_by_l3id(stack, hh->dinfo);
			if (!bc) {
				bc=find_bc_by_masked_l3id(stack, hh->dinfo, 0xffff0000);
				if (bc) { 
					int myprocid=bc->l3_id&0x0000ffff;
					hh->dinfo=(hh->dinfo&0xffff0000)|myprocid;
					cb_log(4,bc->port,"Repaired reject Bug, new dinfo: %x\n",hh->dinfo);
				}
			}
		}
		break;
		
		case CC_RELEASE_COMPLETE|INDICATION:
			break;

		case CC_SUSPEND|INDICATION:
		{
			msg_t *dmsg;
			cb_log(4, stack->port, " --> Got Suspend, sending Reject for now\n");
			dmsg = create_l3msg(CC_SUSPEND_REJECT | REQUEST,MT_SUSPEND_REJECT, hh->dinfo,sizeof(RELEASE_COMPLETE_t), 1);
			stack->nst.manager_l3(&stack->nst, dmsg);
			free_msg(msg);
			return 0;
		}
		break;
		case CC_RESUME|INDICATION:
			break;

		case CC_RELEASE|CONFIRM:
			break;
			
		case CC_RELEASE|INDICATION:
			break;

		case CC_RELEASE_CR|INDICATION:
		{
			struct misdn_bchannel *bc=find_bc_by_l3id(stack, hh->dinfo);
			struct misdn_bchannel dummybc;
			iframe_t frm; /* fake te frm to remove callref from global callreflist */
			frm.dinfo = hh->dinfo;

			frm.addr=stack->upper_id | FLG_MSG_DOWN;

			frm.prim = CC_RELEASE_CR|INDICATION;
			cb_log(4, stack->port, " --> Faking Realease_cr for %x\n",frm.addr);
			/** removing procid **/
			if (!bc) {
				cb_log(4, stack->port, " --> Didn't found BC so temporarly creating dummy BC (l3id:%x) on this port.\n", hh->dinfo);
				memset (&dummybc,0,sizeof(dummybc));
				dummybc.port=stack->port;
				dummybc.l3_id=hh->dinfo;
				bc=&dummybc; 
			}
	
			if (bc) {
				if ( (bc->l3_id & 0xff00) == 0xff00) {
					cb_log(4, stack->port, " --> Removing Process Id:%x on this port.\n", bc->l3_id&0xff);
					stack->procids[bc->l3_id&0xff] = 0 ;
				}
			}
			else cb_log(-1, stack->port, "Couldnt find BC so I couldnt remove the Process!!!! this is a bad port.\n");
	
			handle_cr(stack, &frm);
			free_msg(msg);
			return 0 ;
		}
		break;
      
		case CC_NEW_CR|INDICATION:
			/*  Got New CR for bchan, for now I handle this one in */
			/*  connect_ack, Need to be changed */
		{
			struct misdn_bchannel *bc=find_bc_by_l3id(stack, hh->dinfo);
			int l3id = *((int *)(((u_char *)msg->data)+ mISDNUSER_HEAD_SIZE));
			if (!bc) { cb_log(-1, stack->port, " --> In NEW_CR: didn't found bc ??\n"); return -1;};
			if (((l3id&0xff00)!=0xff00) && ((bc->l3_id&0xff00)==0xff00)) {
				cb_log(4, stack->port, " --> Removing Process Id:%x on this port.\n", 0xff&bc->l3_id);
				stack->procids[bc->l3_id&0xff] = 0 ;
			}
			cb_log(4, stack->port, "lib: Event_ind:CC_NEW_CR : very new L3ID  is %x\n",l3id );
	
			bc->l3_id =l3id;
			cb_event(EVENT_NEW_L3ID, bc, glob_mgr->user_data);
	
			free_msg(msg);
			return 0;
		}
      
		case DL_ESTABLISH | INDICATION:
		case DL_ESTABLISH | CONFIRM:
		{
			cb_log(4, stack->port, "%% GOT L2 Activate Info.\n");
			stack->l2link = 1;
			
			free_msg(msg);
			return 0;
		}
		break;


		case DL_RELEASE | INDICATION:
		case DL_RELEASE | CONFIRM:
		{
			cb_log(4, stack->port, "%% GOT L2 DeActivate Info.\n");
			stack->l2link = 0;
			
			free_msg(msg);
			return 0;
		}
		break;
		}
	}
	
	{
		/*  Parse Events and fire_up to App. */
		struct misdn_bchannel *bc;
		struct misdn_bchannel dummybc;
		
		enum event_e event = isdn_msg_get_event(msgs_g, msg, 1);
    
		bc=find_bc_by_l3id(stack, hh->dinfo);
    
		if (!bc) {
      
			cb_log(4, stack->port, " --> Didn't found BC so temporarly creating dummy BC (l3id:%x).\n", hh->dinfo);
			memset (&dummybc,0,sizeof(dummybc));
			dummybc.port=stack->port;
			dummybc.l3_id=hh->dinfo;
			bc=&dummybc; 
		}
		if (bc ) {
			isdn_msg_parse_event(msgs_g,msg,bc, 1);

			switch (event) {
				case EVENT_SETUP:
					if (bc->channel>0 && bc->channel<255) {

						if (stack->ptp) 
							set_chan_in_stack(stack, bc->channel);
						else 
							cb_log(-1,stack->port," --> PTMP but channel requested\n");

					} else {

						bc->channel = find_free_chan_in_stack(stack, 0);
						if (!bc->channel) {
							cb_log(-1, stack->port, " No free channel at the moment\n");
					
							msg_t *dmsg;
				
							cb_log(-1, stack->port, "Releaseing call %x (No free Chan for you..)\n", hh->dinfo);
								dmsg = create_l3msg(CC_RELEASE_COMPLETE | REQUEST,MT_RELEASE_COMPLETE, hh->dinfo,sizeof(RELEASE_COMPLETE_t), 1);
							stack->nst.manager_l3(&stack->nst, dmsg);
							free_msg(msg);
							return 0;
						}
						
					}
#if 0
					setup_bc(bc);
#endif

					break;
				case EVENT_RELEASE:
				case EVENT_RELEASE_COMPLETE:
					clean_up_bc(bc);
					break;

				default:
				break;
			}
			
			if(!isdn_get_info(msgs_g,event,1)) {
				cb_log(4, stack->port, "Unknown Event Ind: prim %x dinfo %x\n",hh->prim, hh->dinfo);
			} else {
				if (reject) {
					switch(bc->cause){
						case 17:
							cb_log(1, stack->port, "Siemens Busy reject..\n");

							break;
						default:
							return 0;
					}
				}
				cb_event(event, bc, glob_mgr->user_data);
			}
      
		} else {
			cb_log(4, stack->port, "No BC found with l3id: prim %x dinfo %x\n",hh->prim, hh->dinfo);
		}

		free_msg(msg);
	}


	return 0;
}


int handle_timers(msg_t* msg)
{
	iframe_t *frm= (iframe_t*)msg->data;
	struct misdn_stack *stack; 
  
	/* Timer Stuff */
	switch (frm->prim) {
	case MGR_INITTIMER | CONFIRM:
	case MGR_ADDTIMER | CONFIRM:
	case MGR_DELTIMER | CONFIRM:
	case MGR_REMOVETIMER | CONFIRM:
		free_msg(msg);
		return(1);
	}
  
  
  
	if (frm->prim==(MGR_TIMER | INDICATION) ) {
		for (stack = glob_mgr->stack_list;
		     stack;
		     stack = stack->next) {
			itimer_t *it;
      
			if (!stack->nt) continue;
      
			it = stack->nst.tlist;
			/* find timer */
			for(it=stack->nst.tlist;
			    it;
			    it=it->next) {
				if (it->id == (int)frm->addr)
					break;
			}
			if (it) {
				int ret;
				ret = mISDN_write_frame(stack->midev, msg->data, frm->addr,
							MGR_TIMER | RESPONSE, 0, 0, NULL, TIMEOUT_1SEC);
				test_and_clear_bit(FLG_TIMER_RUNING, (long unsigned int *)&it->Flags);
				ret = it->function(it->data);
				free_msg(msg);
				return 1;
			}
		}
    
		cb_log(-1, 0, "Timer Msg without Timer ??\n");
		free_msg(msg);
		return 1;
	}
  
	return 0;
}



void misdn_lib_tone_generator_start(struct misdn_bchannel *bc)
{
	bc->generate_tone=1;
}

void misdn_lib_tone_generator_stop(struct misdn_bchannel *bc)
{
	bc->generate_tone=0;
}


static int do_tone(struct misdn_bchannel *bc, int len)
{
	bc->tone_cnt=len;
	
	if (bc->generate_tone) {
		cb_event(EVENT_TONE_GENERATE, bc, glob_mgr->user_data);
		
		if ( !bc->nojitter ) {
			misdn_tx_jitter(bc,len);
		}
		
		return 1;
	}
	
	return 0;
}



void misdn_tx_jitter(struct misdn_bchannel *bc, int len)
{
	char buf[4096 + mISDN_HEADER_LEN];
	char *data=&buf[mISDN_HEADER_LEN];
	iframe_t *txfrm= (iframe_t*)buf;
	int jlen, r;
	
	jlen=cb_jb_empty(bc,data,len);
	
	if (jlen) {
		flip_buf_bits( data, jlen);
		
		if (jlen < len) {
			cb_log(7,bc->port,"Jitterbuffer Underrun.\n");
		}
		
		txfrm->prim = DL_DATA|REQUEST;
		
		txfrm->dinfo = 0;
		
		txfrm->addr = bc->addr|FLG_MSG_DOWN; /*  | IF_DOWN; */
		
		txfrm->len =jlen;
		cb_log(9, bc->port, "Transmitting %d samples 2 misdn\n", txfrm->len);
		
		r=mISDN_write( glob_mgr->midev, buf, txfrm->len + mISDN_HEADER_LEN, 8000 );
	} else {
#define MISDN_GEN_SILENCE
#ifdef MISDN_GEN_SILENCE
		int cnt=len/TONE_SILENCE_SIZE;
		int rest=len%TONE_SILENCE_SIZE;
		int i;

		for (i=0; i<cnt; i++) {
			memcpy(data, tone_silence_flip, TONE_SILENCE_SIZE );
			data +=TONE_SILENCE_SIZE;
		}

		if (rest) {
			memcpy(data, tone_silence_flip, rest);
		}

		txfrm->prim = DL_DATA|REQUEST;

		txfrm->dinfo = 0;

		txfrm->addr = bc->addr|FLG_MSG_DOWN; /*  | IF_DOWN; */

		txfrm->len =len;
		cb_log(9, bc->port, "Transmitting %d samples 2 misdn\n", txfrm->len);

		r=mISDN_write( glob_mgr->midev, buf, txfrm->len + mISDN_HEADER_LEN, 8000 );
#endif

	}
}

int handle_bchan(msg_t *msg)
{
	iframe_t *frm= (iframe_t*)msg->data;


	struct misdn_bchannel *bc=find_bc_by_addr(frm->addr);
	
	if (!bc) {
		cb_log(1,0,"handle_bchan: BC not found for prim:%x with addr:%x dinfo:%x\n", frm->prim, frm->addr, frm->dinfo);
		return 0 ;
	}
	
	struct misdn_stack *stack=get_stack_by_bc(bc);
	
	if (!stack) {
		cb_log(0, bc->port,"handle_bchan: STACK not found for prim:%x with addr:%x dinfo:%x\n", frm->prim, frm->addr, frm->dinfo);
		return 0;
	}
	
	switch (frm->prim) {

	case MGR_SETSTACK| CONFIRM:
		cb_log(2, stack->port, "BCHAN: MGR_SETSTACK|CONFIRM pid:%d\n",bc->pid);
		break;
		
	case MGR_SETSTACK| INDICATION:
		cb_log(2, stack->port, "BCHAN: MGR_SETSTACK|IND pid:%d\n",bc->pid);
	break;
#if 0
	AGAIN:
		bc->addr = mISDN_get_layerid(stack->midev, bc->b_stid, bc->layer);
		if (!bc->addr) {

			if (errno == EAGAIN) {
				usleep(1000);
				goto AGAIN;
			}
			
			cb_log(0,stack->port,"$$$ Get Layer (%d) Id Error: %s\n",bc->layer,strerror(errno));
			
			/* we kill the channel later, when we received some
			   data. */
			bc->addr= frm->addr;
		} else if ( bc->addr < 0) {
			cb_log(-1, stack->port,"$$$ bc->addr <0 Error:%s\n",strerror(errno));
			bc->addr=0;
		}
		
		cb_log(4, stack->port," --> Got Adr %x\n", bc->addr);

		free_msg(msg);
	
		
		switch(bc->bc_state) {
		case BCHAN_SETUP:
			bc_state_change(bc,BCHAN_SETUPED);
		break;

		case BCHAN_CLEAN_REQUEST:
		default:
			cb_log(-1, stack->port," --> STATE WASN'T SETUP (but %s) in SETSTACK|IND pid:%d\n",bc_state2str(bc->bc_state), bc->pid);
			clean_up_bc(bc);
		}
		return 1;
#endif

	case MGR_DELLAYER| INDICATION:
		cb_log(2, stack->port, "BCHAN: MGR_DELLAYER|IND pid:%d\n",bc->pid);
		break;
		
	case MGR_DELLAYER| CONFIRM:
		cb_log(2, stack->port, "BCHAN: MGR_DELLAYER|CNF pid:%d\n",bc->pid);
		
		bc->pid=0;
		bc->addr=0;
		
		free_msg(msg);
		return 1;
		
	case PH_ACTIVATE | INDICATION:
	case DL_ESTABLISH | INDICATION:
		cb_log(2, stack->port, "BCHAN: ACT Ind pid:%d\n", bc->pid);

		free_msg(msg);
		return 1;    

	case PH_ACTIVATE | CONFIRM:
	case DL_ESTABLISH | CONFIRM:
		
		cb_log(2, stack->port, "BCHAN: bchan ACT Confirm pid:%d\n",bc->pid);
		free_msg(msg);
		
		return 1;    

	case DL_ESTABLISH | REQUEST:
		{
			char buf[128];
			mISDN_write_frame(stack->midev, buf, bc->addr | FLG_MSG_TARGET | FLG_MSG_DOWN,  DL_ESTABLISH | CONFIRM, 0,0, NULL, TIMEOUT_1SEC);
		}
		free_msg(msg);
		return 1;

	case DL_RELEASE|REQUEST:
		{
			char buf[128];
			mISDN_write_frame(stack->midev, buf, bc->addr | FLG_MSG_TARGET | FLG_MSG_DOWN,  DL_RELEASE| CONFIRM, 0,0, NULL, TIMEOUT_1SEC);
		}
		free_msg(msg);
		return 1;
		
	case PH_DEACTIVATE | INDICATION:
	case DL_RELEASE | INDICATION:
		cb_log (2, stack->port, "BCHAN: DeACT Ind pid:%d\n",bc->pid);
		
		free_msg(msg);
		return 1;
    
	case PH_DEACTIVATE | CONFIRM:
	case DL_RELEASE | CONFIRM:
		cb_log(4, stack->port, "BCHAN: DeACT Conf pid:%d\n",bc->pid);
		
		free_msg(msg);
		return 1;
    
	case PH_CONTROL|INDICATION:
	{
		unsigned int cont = *((unsigned int *)&frm->data.p);
		
		cb_log(4, stack->port, "PH_CONTROL: channel:%d oad%d:%s dad%d:%s \n", bc->channel, bc->onumplan,bc->oad, bc->dnumplan,bc->dad);

		if ((cont&~DTMF_TONE_MASK) == DTMF_TONE_VAL) {
			int dtmf = cont & DTMF_TONE_MASK;
			cb_log(4, stack->port, " --> DTMF TONE: %c\n",dtmf);
			bc->dtmf=dtmf;
			cb_event(EVENT_DTMF_TONE, bc, glob_mgr->user_data);
	
			free_msg(msg);
			return 1;
		}
		if (cont == BF_REJECT) {
			cb_log(4, stack->port, " --> BF REJECT\n");
			free_msg(msg);
			return 1;
		}
		if (cont == BF_ACCEPT) {
			cb_log(4, stack->port, " --> BF ACCEPT\n");
			free_msg(msg);
			return 1;
		}
	}
	break;

	case PH_DATA|REQUEST:
	case DL_DATA|REQUEST:
		cb_log(0, stack->port, "DL_DATA REQUEST \n");
		do_tone(bc, 64);
		
		free_msg(msg);
		return 1;
	
	
	case PH_DATA|INDICATION:
	case DL_DATA|INDICATION:
	{
		bc->bframe = (void*)&frm->data.i;
		bc->bframe_len = frm->len;

		/** Anyway flip the bufbits **/
		if ( misdn_cap_is_speech(bc->capability) ) 
			flip_buf_bits(bc->bframe, bc->bframe_len);
	

		if (!bc->bframe_len) {
			cb_log(2, stack->port, "DL_DATA INDICATION bc->addr:%x frm->addr:%x\n", bc->addr, frm->addr);
			free_msg(msg);
			return 1;
		}

		if ( (bc->addr&STACK_ID_MASK) != (frm->addr&STACK_ID_MASK) ) {
			cb_log(2, stack->port, "DL_DATA INDICATION bc->addr:%x frm->addr:%x\n", bc->addr, frm->addr);
			free_msg(msg);
			return 1;
		}
		
#if MISDN_DEBUG
		cb_log(-1, stack->port, "DL_DATA INDICATION Len %d\n", frm->len);

#endif
		
		if ( (bc->bc_state == BCHAN_ACTIVATED) && frm->len > 0) {
			int t;

#ifdef MISDN_B_DEBUG
			cb_log(0,bc->port,"do_tone START\n");
#endif
			t=do_tone(bc,frm->len);

#ifdef MISDN_B_DEBUG
			cb_log(0,bc->port,"do_tone STOP (%d)\n",t);
#endif
			if (  !t ) {
				
				if ( misdn_cap_is_speech(bc->capability)) {
					if ( !bc->nojitter ) {
#ifdef MISDN_B_DEBUG
						cb_log(0,bc->port,"tx_jitter START\n");
#endif
						misdn_tx_jitter(bc,frm->len);
#ifdef MISDN_B_DEBUG
						cb_log(0,bc->port,"tx_jitter STOP\n");
#endif
					}
				}

#ifdef MISDN_B_DEBUG	
				cb_log(0,bc->port,"EVENT_B_DATA START\n");
#endif
				
				int i=cb_event( EVENT_BCHAN_DATA, bc, glob_mgr->user_data);
#ifdef MISDN_B_DEBUG	
				cb_log(0,bc->port,"EVENT_B_DATA STOP\n");
#endif
				
				if (i<0) {
					cb_log(10,stack->port,"cb_event returned <0\n");
					/*clean_up_bc(bc);*/
				}
			}
		}
		free_msg(msg);
		return 1;
	}


	case PH_CONTROL | CONFIRM:
		cb_log(2, stack->port, "PH_CONTROL|CNF bc->addr:%x\n", frm->addr);
		free_msg(msg);
		return 1;

	case PH_DATA | CONFIRM:
	case DL_DATA|CONFIRM:
#if MISDN_DEBUG

		cb_log(-1, stack->port, "Data confirmed\n");

#endif
		free_msg(msg);
		
		return 1;
	case DL_DATA|RESPONSE:
#if MISDN_DEBUG
		cb_log(-1, stack->port, "Data response\n");

#endif
		break;
	}
  
	return 0;
}



int handle_frm_nt(msg_t *msg)
{
	iframe_t *frm= (iframe_t*)msg->data;
	struct misdn_stack *stack;
	int err=0;

	stack=find_stack_by_addr( frm->addr );

	
  
	if (!stack || !stack->nt) {
		return 0;
	}

	
	if ((err=stack->nst.l1_l2(&stack->nst,msg))) {
    
		if (nt_err_cnt > 0 ) {
			if (nt_err_cnt < 100) {
				nt_err_cnt++; 
				cb_log(-1, stack->port, "NT Stack sends us error: %d \n", err);
			} else if (nt_err_cnt < 105){
				cb_log(-1, stack->port, "NT Stack sends us error: %d over 100 times, so I'll stop this message\n", err);
				nt_err_cnt = - 1; 
			}
		}
		free_msg(msg);
		return 1;
		
	}
	
	return 1;
}


int handle_frm(msg_t *msg)
{
	iframe_t *frm = (iframe_t*) msg->data;
	
	struct misdn_stack *stack=find_stack_by_addr(frm->addr);

	
	cb_log(4,stack?stack->port:0,"handle_frm: frm->addr:%x frm->prim:%x\n",frm->addr,frm->prim);
	
  
	if (!stack || stack->nt) {
		return 0;
	}

	{
		struct misdn_bchannel *bc;
		int ret=handle_cr(stack, frm);

		if (ret<0) {
			cb_log(3,stack?stack->port:0,"handle_frm: handle_cr <0 prim:%x addr:%x\n", frm->prim, frm->addr);
		}

		if(ret) {
			free_msg(msg);
			return 1;
		}
    
		bc=find_bc_by_l3id(stack, frm->dinfo);
    
		if (bc ) {
			enum event_e event = isdn_msg_get_event(msgs_g, msg, 0);
			enum event_response_e response=RESPONSE_OK;
      
			isdn_msg_parse_event(msgs_g,msg,bc, 0);
			
			/** Preprocess some Events **/
			handle_event(bc, event, frm);
			/*  shoot up event to App: */
			cb_log(5, stack->port, "lib Got Prim: Addr %x prim %x dinfo %x\n",frm->addr, frm->prim, frm->dinfo);
      
			if(!isdn_get_info(msgs_g,event,0)) 
				cb_log(-1, stack->port, "Unknown Event Ind: Addr:%x prim %x dinfo %x\n",frm->addr, frm->prim, frm->dinfo);
			else 
				response=cb_event(event, bc, glob_mgr->user_data);
#if 1
			if (event == EVENT_SETUP) {
				switch (response) {
				case RESPONSE_IGNORE_SETUP_WITHOUT_CLOSE:

					cb_log(-1, stack->port, "TOTALY IGNORING SETUP \n");					
					
					break;
				case RESPONSE_IGNORE_SETUP:
					/* I think we should send CC_RELEASE_CR, but am not sure*/

					bc->out_cause=16;
					misdn_lib_send_event(bc,EVENT_RELEASE_COMPLETE);
					empty_chan_in_stack(stack, bc->channel);
					empty_bc(bc);
					bc_state_change(bc,BCHAN_CLEANED);

					cb_log(-1, stack->port, "GOT IGNORE SETUP\n");

					
					break;
				case RESPONSE_OK:
					cb_log(4, stack->port, "GOT SETUP OK\n");

					
					break;
				default:
					break;
				}
			}

			cb_log(5, stack->port, "Freeing Msg on prim:%x \n",frm->prim);

			
			free_msg(msg);
			return 1;
#endif
      
		} else {
			cb_log(-1, stack->port, "NO BC FOR STACK\n");		
		}
	}

	cb_log(4, stack->port, "TE_FRM_HANDLER: Returning 0 on prim:%x \n",frm->prim);
	return 0;
}


int handle_l1(msg_t *msg)
{
	iframe_t *frm = (iframe_t*) msg->data;
	struct misdn_stack *stack = find_stack_by_addr(frm->addr);
	int i ;
	
	if (!stack) return 0 ;
  
	switch (frm->prim) {
	case PH_ACTIVATE | CONFIRM:
	case PH_ACTIVATE | INDICATION:
		cb_log (1, stack->port, "L1: PH L1Link Up!\n");
		stack->l1link=1;
		
		if (stack->nt) {
			
			if (stack->nst.l1_l2(&stack->nst, msg))
				free_msg(msg);
		} else {
			free_msg(msg);
		}
		
		for (i=0;i<stack->b_num; i++) {
			if (stack->bc[i].evq != EVENT_NOTHING) {
				cb_log(4, stack->port, "Fireing Queued Event %s because L1 got up\n", isdn_get_info(msgs_g, stack->bc[i].evq, 0));
				misdn_lib_send_event(&stack->bc[i],stack->bc[i].evq);
				stack->bc[i].evq=EVENT_NOTHING;
			}
			
		}
		return 1;

	case PH_ACTIVATE | REQUEST:
		free_msg(msg);
		cb_log(1,stack->port,"L1: PH_ACTIVATE|REQUEST \n");
		return 1;
		
	case PH_DEACTIVATE | REQUEST:
		free_msg(msg);
		cb_log(1,stack->port,"L1: PH_DEACTIVATE|REQUEST \n");
		return 1;
		
	case PH_DEACTIVATE | CONFIRM:
	case PH_DEACTIVATE | INDICATION:
		cb_log (1, stack->port, "L1: PH L1Link Down! \n");
		
		for (i=0; i<stack->b_num; i++) {
			if (global_state == MISDN_INITIALIZED)  {
				cb_event(EVENT_CLEANUP, &stack->bc[i], glob_mgr->user_data);
			}
		}
		
		if (stack->nt) {
			if (stack->nst.l1_l2(&stack->nst, msg))
				free_msg(msg);
		} else {
			free_msg(msg);
		}
		
		stack->l1link=0;
		stack->l2link=0;
		
		return 1;
	}
  
	return 0;
}

int handle_l2(msg_t *msg)
{
	iframe_t *frm = (iframe_t*) msg->data;

	struct misdn_stack *stack = find_stack_by_addr(frm->addr);
	
	if (!stack) {
		return 0 ;
	}
	
	switch(frm->prim) {

	case DL_ESTABLISH | REQUEST:
		cb_log(1,stack->port,"DL_ESTABLISH|REQUEST \n");
		return 1;
	case DL_RELEASE | REQUEST:
		cb_log(1,stack->port,"DL_RELEASE|REQUEST \n");
		return 1;
		
	case DL_ESTABLISH | INDICATION:
	case DL_ESTABLISH | CONFIRM:
	{
		cb_log (3, stack->port, "L2: L2Link Up! \n");
		stack->l2link=1;
		free_msg(msg);
		return 1;
	}
	break;
    
	case DL_RELEASE | INDICATION:
	case DL_RELEASE | CONFIRM:
	{
		cb_log (3, stack->port, "L2: L2Link Down! \n");
		stack->l2link=0;
		
		free_msg(msg);
		return 1;
	}
	break;
	}
	return 0;
}

int handle_mgmt(msg_t *msg)
{
	iframe_t *frm = (iframe_t*) msg->data;

	if ( (frm->addr == 0) && (frm->prim == (MGR_DELLAYER|CONFIRM)) ) {
		cb_log(2, 0, "MGMT: DELLAYER|CONFIRM Addr: 0 !\n") ;
		free_msg(msg);
		return 1;
	}
	
	struct misdn_stack * stack=find_stack_by_addr(frm->addr);
	
	if (!stack) {
		if (frm->prim == (MGR_DELLAYER|CONFIRM)) {
			cb_log(2, 0, "MGMT: DELLAYER|CONFIRM Addr: %x !\n",
					frm->addr) ;
			free_msg(msg);
			return 1;
		}
		
		return 0;
	}
	
	switch(frm->prim) {
	case MGR_SHORTSTATUS | INDICATION:
	case MGR_SHORTSTATUS | CONFIRM:
		cb_log(2, 0, "MGMT: Short status dinfo %x\n",frm->dinfo);
		
		switch (frm->dinfo) {
		case SSTATUS_L1_ACTIVATED:
			cb_log(1, 0, "MGMT: SSTATUS: L1_ACTIVATED \n");
			stack->l1link=1;
		
			break;
		case SSTATUS_L1_DEACTIVATED:
			cb_log(1, 0, "MGMT: SSTATUS: L1_DEACTIVATED \n");
			stack->l1link=0;

			clear_l3(stack);
			break;

		case SSTATUS_L2_ESTABLISHED:
			cb_log(1, stack->port, "MGMT: SSTATUS: L2_ESTABLISH \n");
			stack->l2link=1;
			break;
			
		case SSTATUS_L2_RELEASED:
			cb_log(1, stack->port, "MGMT: SSTATUS: L2_RELEASED \n");
			stack->l2link=0;
			break;
		}
		
		free_msg(msg);
		return 1;
		
	case MGR_SETSTACK | INDICATION:
		cb_log(2, stack->port, "MGMT: SETSTACK|IND dinfo %x\n",frm->dinfo);
		free_msg(msg);
		return 1;
	case MGR_DELLAYER | CONFIRM:
		cb_log(2, stack->port, "MGMT: DELLAYER|CNF dinfo %x\n",frm->dinfo) ;
		free_msg(msg);
		return 1;
		
	}
	
	/*
	if ( (frm->prim & 0x0f0000) ==  0x0f0000) {
	cb_log(5, 0, "$$$ MGMT FRAME: prim %x addr %x dinfo %x\n",frm->prim, frm->addr, frm->dinfo) ;
	free_msg(msg);
	return 1;
	} */
    
	return 0;
}


msg_t *fetch_msg(int midev) 
{
	msg_t *msg=alloc_msg(MAX_MSG_SIZE);
	int r;
/*	fd_set rdfs; */

	if (!msg) {
		cb_log(-1, 0, "fetch_msg: alloc msg failed !!");
		return NULL;
	}

#if 0
	FD_ZERO(&rdfs);
	FD_SET(midev,&rdfs);
  
	mISDN_select(FD_SETSIZE, &rdfs, NULL, NULL, NULL);
	//select(FD_SETSIZE, &rdfs, NULL, NULL, NULL);
  
	if (FD_ISSET(midev, &rdfs)) {
#endif

	AGAIN:
		r=mISDN_read(midev,msg->data,MAX_MSG_SIZE, TIMEOUT_10SEC);
		msg->len=r;
    
		if (r==0) {
			free_msg(msg); /* danger, cauz usualy freeing in main_loop */
			cb_log(6,0,"Got empty Msg..\n");
			return NULL;
		}

		if (r<0) {
			if (errno == EAGAIN) {
				/*we wait for mISDN here*/
				cb_log(4,0,"mISDN_read wants us to wait\n");
				usleep(5000);
				goto AGAIN;
			}
			
			cb_log(-1,0,"mISDN_read returned :%d error:%s (%d)\n",r,strerror(errno),errno); 
		}

		return msg;

#if 0
	} else {
		printf ("Select timeout\n");
	}
#endif
  
	return NULL;
}

static void misdn_lib_isdn_l1watcher(void *arg)
{
	struct misdn_lib *mgr = arg;
	struct misdn_stack *stack;

	while (1) {
		sleep(mgr->l1watcher_timeout);
		
		/* look out for l1 which are down
		   and try to pull the up.

		   We might even try to pull the l2 up in the
		   ptp case.
		*/
		for (stack = mgr->stack_list;
		     stack;
		     stack = stack->next) {
			cb_log(4,stack->port,"Checking L1 State\n");	
			if (!stack->l1link) {
				cb_log(4,stack->port,"L1 State Down, trying to get it up again\n");	
				misdn_lib_get_short_status(stack);
				misdn_lib_get_l1_up(stack); 
				misdn_lib_get_l2_up(stack); 
			}
		}
	}
}

static void misdn_lib_isdn_event_catcher(void *arg)
{
	struct misdn_lib *mgr = arg;
	int zero_frm=0 , fff_frm=0 ;
	int midev= mgr->midev;
	int port=0;
	
	while (1) {
		msg_t *msg = fetch_msg(midev); 
		iframe_t *frm;
		
		
		if (!msg) continue;
		
		frm = (iframe_t*) msg->data;
		
		/** When we make a call from NT2Ast we get this frames **/
		if (frm->len == 0 && frm->addr == 0 && frm->dinfo == 0 && frm->prim == 0 ) {
			zero_frm++; 
			free_msg(msg);
			continue;
		} else {
			if (zero_frm) {
				cb_log(-1, port, "*** Alert: %d zero_frms caught\n", zero_frm);
				zero_frm = 0 ;
			}
		}
		
		/** I get this sometimes after setup_bc **/
		if (frm->len == 0 &&  frm->dinfo == 0 && frm->prim == 0xffffffff ) {
			fff_frm++; 
			free_msg(msg);
			continue;
		} else {
			if (fff_frm) {
				cb_log(-1, port, "*** Alert: %d fff_frms caught\n", fff_frm);
				fff_frm = 0 ;
			}
		}
		
		manager_isdn_handler(frm, msg);
	}

}


/** App Interface **/

int te_lib_init() {
	char buff[1025];
	iframe_t *frm=(iframe_t*)buff;
	int midev=mISDN_open();
	int ret;

	memset(buff,0,1025);
  
	if  (midev<=0) return midev;
  
/* create entity for layer 3 TE-mode */
	mISDN_write_frame(midev, buff, 0, MGR_NEWENTITY | REQUEST, 0, 0, NULL, TIMEOUT_1SEC);
	ret = mISDN_read_frame(midev, frm, sizeof(iframe_t), 0, MGR_NEWENTITY | CONFIRM, TIMEOUT_1SEC);
  
	if (ret < mISDN_HEADER_LEN) {
	noentity:
		fprintf(stderr, "cannot request MGR_NEWENTITY from mISDN: %s\n",strerror(errno));
		exit(-1);
	}
  
	entity = frm->dinfo & 0xffff ;
  
	if (!entity)
		goto noentity;

	return midev;
  
}

void te_lib_destroy(int midev)
{
	char buf[1024];
	mISDN_write_frame(midev, buf, 0, MGR_DELENTITY | REQUEST, entity, 0, NULL, TIMEOUT_1SEC);

	cb_log(4, 0, "Entetity deleted\n");
	mISDN_close(midev);
	cb_log(4, 0, "midev closed\n");
}



void misdn_lib_transfer(struct misdn_bchannel* holded_bc)
{
	holded_bc->holded=0;
}

struct misdn_bchannel *manager_find_bc_by_pid(int pid)
{
	struct misdn_stack *stack;
	int i;
  
	for (stack=glob_mgr->stack_list;
	     stack;
	     stack=stack->next) {
		for (i=0; i<stack->b_num; i++)
			if (stack->bc[i].pid == pid) return &stack->bc[i];
	}
  
	return NULL;
}

struct misdn_bchannel *manager_find_bc_holded(struct misdn_bchannel* bc)
{
	struct misdn_stack *stack=get_stack_by_bc(bc);
	return find_bc_holded(stack);
}



struct misdn_bchannel* misdn_lib_get_free_bc(int port, int channel)
{
	struct misdn_stack *stack;
	int i;
	
	if (channel < 0 || channel > MAX_BCHANS) {
		cb_log(-1,port,"Requested channel out of bounds (%d)\n",channel);
		return NULL;
	}

	for (stack=glob_mgr->stack_list; stack; stack=stack->next) {
    
		if (stack->port == port) {
			if (channel > 0) {
				if (channel <= stack->b_num) {
					for (i = 0; i < stack->b_num; i++) {
						if (stack->bc[i].in_use && stack->bc[i].channel == channel) {
							cb_log(-1,port,"Requested channel:%d on port:%d is already in use\n",channel, port);
							return NULL;
						}
					}
				} else {
					cb_log(-1,port,"Requested channel:%d is out of bounds on port:%d\n",channel, port);
					return NULL;
				}
			}
			for (i = 0; i < stack->b_num; i++) {
				if (!stack->bc[i].in_use) {
					stack->bc[i].channel = channel;
					stack->bc[i].channel_preselected = channel?1:0;
					stack->bc[i].in_use = 1;
					return &stack->bc[i];
				}
			}

			cb_log(-1,port,"There is no free channel on port (%d)\n",port);
			return NULL;
		}
	}

	cb_log(-1,port,"Port is not configured (%d)\n",port);
	return NULL;
}


char *fac2str (enum facility_type type) {
	struct arr_el { 
		enum facility_type p; 
		char *s ; 
	} arr[] = {
		{ FACILITY_NONE, "FAC_NONE" },
		{ FACILITY_CALLDEFLECT, "FAC_CALLDEFLECT"},
		{ FACILITY_CENTREX, "FAC_CENTREX"}
	};
	
	int i;
	
	for (i=0; i < sizeof(arr)/sizeof( struct arr_el) ; i ++)
		if ( arr[i].p==type) return arr[i].s;
	
	return "FAC_UNKNOWN";
}

void misdn_lib_log_ies(struct misdn_bchannel *bc)
{
	if (!bc) return;

	struct misdn_stack *stack=get_stack_by_bc(bc);

	if (!stack) return;

	cb_log(2, stack->port, " --> mode:%s cause:%d ocause:%d rad:%s cad:%s\n", stack->nt?"NT":"TE", bc->cause, bc->out_cause, bc->rad, bc->cad);
	
	cb_log(3, stack->port, " --> facility:%s out_facility:%s\n",fac2str(bc->fac_type),fac2str(bc->out_fac_type));
	
	cb_log(2, stack->port,
	       " --> info_dad:%s onumplan:%c dnumplan:%c rnumplan:%c cpnnumplan:%c\n",
	       bc->info_dad,
	       bc->onumplan>=0?'0'+bc->onumplan:' ',
	       bc->dnumplan>=0?'0'+bc->dnumplan:' ',
	       bc->rnumplan>=0?'0'+bc->rnumplan:' ',
	       bc->cpnnumplan>=0?'0'+bc->cpnnumplan:' '
		);
	cb_log(3, stack->port, " --> screen:%d --> pres:%d\n",
			bc->screen, bc->pres);
	
	cb_log(2, stack->port, " --> channel:%d caps:%s pi:%x keypad:%s sending_complete:%d\n", bc->channel, bearer2str(bc->capability),bc->progress_indicator, bc->keypad, bc->sending_complete);

	cb_log(3, stack->port, " --> urate:%d rate:%d mode:%d user1:%d\n", bc->urate, bc->rate, bc->mode,bc->user1);
	
	cb_log(3, stack->port, " --> pid:%d addr:%x l3id:%x\n", bc->pid, bc->addr, bc->l3_id);
	cb_log(3, stack->port, " --> b_stid:%x layer_id:%x\n", bc->b_stid, bc->layer_id);
	
	cb_log(4, stack->port, " --> bc:%x h:%d sh:%d\n", bc, bc->holded, bc->stack_holder);
}

int misdn_lib_send_event(struct misdn_bchannel *bc, enum event_e event )
{
	msg_t *msg; 
	int err = -1 ;
	int ret=0;
  
	if (!bc) goto ERR; 
	
	struct misdn_stack *stack=get_stack_by_bc(bc);
	
	if (!stack) {
		cb_log(-1,bc->port,"SENDEVENT: no Stack for event:%s oad:%s dad:%s \n", isdn_get_info(msgs_g, event, 0), bc->oad, bc->dad);
		return -1;
	}
	
	cb_log(6,stack->port,"SENDEVENT: stack->nt:%d stack->uperid:%x\n",stack->nt, stack->upper_id);

	if ( stack->nt && !stack->l1link) {
		/** Queue Event **/
		bc->evq=event;
		cb_log(1, stack->port, "Queueing Event %s because L1 is down (btw. Activating L1)\n", isdn_get_info(msgs_g, event, 0));
		misdn_lib_get_l1_up(stack);
		return 0;
	}
	
	cb_log(1, stack->port, "I SEND:%s oad:%s dad:%s pid:%d\n", isdn_get_info(msgs_g, event, 0), bc->oad, bc->dad, bc->pid);
	cb_log(1, stack->port, " --> bc_state:%s\n",bc_state2str(bc->bc_state));
	misdn_lib_log_ies(bc);
	
	switch (event) {
	case EVENT_SETUP:
		if (create_process(glob_mgr->midev, bc)<0) {
			cb_log(-1,  stack->port, " No free channel at the moment @ send_event\n");

			err=-ENOCHAN;
			goto ERR;
		}
#if 0
		ret=setup_bc(bc);
		if (ret == -EINVAL) {
			cb_log(-1,bc->port,"send_event: setup_bc failed\n");
		}
#endif
		break;

	case EVENT_PROGRESS:
	case EVENT_ALERTING:
	case EVENT_PROCEEDING:
	case EVENT_SETUP_ACKNOWLEDGE:
		if (!bc->nt && !stack->ptp) break;

	case EVENT_CONNECT:
	case EVENT_RETRIEVE_ACKNOWLEDGE:
		if (stack->nt) {
			if (bc->channel <=0 ) { /*  else we have the channel already */
				bc->channel = find_free_chan_in_stack(stack, bc, 0);
				if (!bc->channel) {
					cb_log(-1, stack->port, " No free channel at the moment\n");
					
					err=-ENOCHAN;
					goto ERR;
				}
			}
			/* Its that i generate channels */
		}
		
		ret=setup_bc(bc);
		if (ret == -EINVAL) {
			cb_log(-1,bc->port,"send_event: setup_bc failed\n");
		}
		
		if ( (event == EVENT_CONNECT ) && misdn_cap_is_speech(bc->capability) ) {
			if ( *bc->crypt_key ) {
				cb_log(4, stack->port,  " --> ENABLING BLOWFISH channel:%d oad%d:%s dad%d:%s \n", bc->channel, bc->onumplan,bc->oad, bc->dnumplan,bc->dad);
				
				manager_ph_control_block(bc,  BF_ENABLE_KEY, bc->crypt_key, strlen(bc->crypt_key) );
			}
			
			if (!bc->nodsp) manager_ph_control(bc,  DTMF_TONE_START, 0);
			
			if (bc->ec_enable) manager_ec_enable(bc);
			
			if (bc->txgain != 0) {
				cb_log(4, stack->port,  "--> Changing txgain to %d\n", bc->txgain);
				manager_ph_control(bc, VOL_CHANGE_TX, bc->txgain);
			}
			
			if ( bc->rxgain != 0 ) {
				cb_log(4, stack->port,  "--> Changing rxgain to %d\n", bc->rxgain);
				manager_ph_control(bc, VOL_CHANGE_RX, bc->rxgain);
			}
			
		}
		
		if (event == EVENT_RETRIEVE_ACKNOWLEDGE) {
			cb_log(0,bc->port,"DO WE NEED TO DO SOMETHING HERE WITH THE BC ?\n");
		}
		break;

	case EVENT_HOLD_ACKNOWLEDGE:
	{
		struct misdn_bchannel *holded_bc=malloc(sizeof(struct misdn_bchannel));
		memcpy(holded_bc,bc,sizeof(struct misdn_bchannel));
		holded_bc->holded=1;
		stack_holder_add(stack,holded_bc);
		
		if (stack->nt) {
			if (bc->bc_state == BCHAN_BRIDGED) {
				misdn_split_conf(bc,bc->conf_id);
				misdn_split_conf(bc->holded_bc,bc->holded_bc->conf_id);
			}

			empty_chan_in_stack(stack,bc->channel);
			empty_bc(bc);
			clean_up_bc(bc);
		}
		
		/** we set it up later at RETRIEVE_ACK again.**/
		/*holded_bc->upset=0;
		  holded_bc->active=0;*/
		bc_state_change(holded_bc,BCHAN_CLEANED);
		
		cb_event( EVENT_NEW_BC, holded_bc, glob_mgr->user_data);
	}
	break;
	
	case EVENT_RELEASE:
	case EVENT_RELEASE_COMPLETE:
		/*we do the cleanup in EVENT_CLEANUP*/
		/*clean_up_bc(bc);*/
		break;
    
	case EVENT_CONNECT_ACKNOWLEDGE:

		if ( bc->nt || misdn_cap_is_speech(bc->capability)) {
			int ret=setup_bc(bc);
			if (ret == -EINVAL){
				cb_log(-1,bc->port,"send_event: setup_bc failed\n");
				
			}
		}
	
		
		if (misdn_cap_is_speech(bc->capability)) {
			if (  !bc->nodsp) manager_ph_control(bc,  DTMF_TONE_START, 0);
			if (bc->ec_enable) manager_ec_enable(bc);
			if ( bc->txgain != 0 ) {
				cb_log(4, stack->port, "--> Changing txgain to %d\n", bc->txgain);
				manager_ph_control(bc, VOL_CHANGE_TX, bc->txgain);
			}
			if ( bc->rxgain != 0 ) {
				cb_log(4, stack->port, "--> Changing rxgain to %d\n", bc->rxgain);
				manager_ph_control(bc, VOL_CHANGE_RX, bc->rxgain);
			}
		}
		break;
    
	default:
		break;
	}
  
	/* Later we should think about sending bchannel data directly to misdn. */
	msg = isdn_msg_build_event(msgs_g, bc, event, stack->nt);
	msg_queue_tail(&stack->downqueue, msg);
	sem_post(&glob_mgr->new_msg);
  
	return 0;
  
 ERR:
	return -1; 
}


int handle_err(msg_t *msg)
{
	iframe_t *frm = (iframe_t*) msg->data;


	if (!frm->addr) {
		static int cnt=0;
		if (!cnt)
			cb_log(0,0,"mISDN Msg without Address pr:%x dinfo:%x\n",frm->prim,frm->dinfo);
		cnt++;
		if (cnt>100) {
			cb_log(0,0,"mISDN Msg without Address pr:%x dinfo:%x (already more than 100 of them)\n",frm->prim,frm->dinfo);
			cnt=0;
		}
		
		free_msg(msg);
		return 1;
		
	}
	
	switch (frm->prim) {
		case MGR_SETSTACK|INDICATION:
			return handle_bchan(msg);
		break;

		case MGR_SETSTACK|CONFIRM:
		case MGR_CLEARSTACK|CONFIRM:
			free_msg(msg) ; 
			return 1;
		break;

		case DL_DATA|CONFIRM:
			cb_log(4,0,"DL_DATA|CONFIRM\n");
			free_msg(msg);
			return 1;

		case PH_CONTROL|CONFIRM:
			cb_log(4,0,"PH_CONTROL|CONFIRM\n");
			free_msg(msg);
			return 1;

		case DL_DATA|INDICATION:
		{
			int port=(frm->addr&MASTER_ID_MASK) >> 8;
			int channel=(frm->addr&CHILD_ID_MASK) >> 16;

			/*we flush the read buffer here*/
			
			cb_log(9,0,"BCHAN DATA without BC: addr:%x port:%d channel:%d\n",frm->addr, port,channel);
			
			free_msg(msg) ; 
			return 1;
			
			
			struct misdn_bchannel *bc=find_bc_by_channel( port , channel);

			if (!bc) {
				struct misdn_stack *stack=find_stack_by_port( port );

				if (!stack) {
					cb_log(-1,0," --> stack not found\n");
					free_msg(msg);
					return 1;
				}
				
				cb_log(-1,0," --> bc not found by channel\n");
				if (stack->l2link)
					misdn_lib_get_l2_down(stack);

				if (stack->l1link)
					misdn_lib_get_l1_down(stack);

				free_msg(msg);
				return 1;
			}
			
			cb_log(3,port," --> BC in state:%s\n", bc_state2str(bc->bc_state));
		}
	}

	return 0;
}


int queue_l2l3(msg_t *msg) {
	iframe_t *frm= (iframe_t*)msg->data;
	struct misdn_stack *stack;
	stack=find_stack_by_addr( frm->addr );

	
	if (!stack) {
		return 0;
	}

	msg_queue_tail(&stack->upqueue, msg);
	sem_post(&glob_mgr->new_msg);
	return 1;
}

int manager_isdn_handler(iframe_t *frm ,msg_t *msg)
{  

	if (frm->dinfo==(signed long)0xffffffff && frm->prim==(PH_DATA|CONFIRM)) {
		cb_log(0,0,"SERIOUS BUG, dinfo == 0xffffffff, prim == PH_DATA | CONFIRM !!!!\n");
	}

	if ( ((frm->addr | ISDN_PID_BCHANNEL_BIT )>> 28 ) == 0x5) {
#ifdef MISDN_HANDLER_DEBUG
		cb_log(0,0,"handle_bchan START\n");
#endif
		if (handle_bchan(msg)) {
#ifdef MISDN_HANDLER_DEBUG
			cb_log(0,0,"handle_bchan STOP\n");
#endif
			return 0 ;
		}
#ifdef MISDN_HANDLER_DEBUG
		cb_log(0,0,"handle_bchan NOTSTOP\n");
#endif	
	}	
	
	if (handle_timers(msg)) 
		return 0 ;
	
	if (handle_mgmt(msg)) 
		return 0 ; 
	
	if (handle_l2(msg)) 
		return 0 ;

	/* Its important to handle l1 AFTER l2  */
	if (handle_l1(msg)) 
		return 0 ;
	
#ifdef MISDN_HANDLER_DEBUG
	cb_log(0,0,"handle_frm_nt START\n");
#endif
	if (handle_frm_nt(msg)) {
#ifdef MISDN_HANDLER_DEBUG
		cb_log(0,0,"handle_frm_nt STOP\n");
#endif
		return 0;
	}
#ifdef MISDN_HANDLER_DEBUG
	cb_log(0,0,"handle_frm_nt NOTSTOP\n");
	
	cb_log(0,0,"handle_frm START\n");
#endif
	
	if (handle_frm(msg)) {
#ifdef MISDN_HANDLER_DEBUG
		cb_log(0,0,"handle_frm STOP\n");
#endif
		
		return 0;
	}
#ifdef MISDN_HANDLER_DEBUG
	cb_log(0,0,"handle_frm NOTSTOP\n");	
	
	cb_log(0,0,"handle_err START\n");
#endif
	
	if (handle_err(msg)) {
#ifdef MISDN_HANDLER_DEBUG
		cb_log(0,0,"handle_err STOP\n");
#endif
		return 0 ;
	}
#ifdef MISDN_HANDLER_DEBUG
	cb_log(0,0,"handle_err NOTSTOP\n");
#endif

	
	cb_log(-1, 0, "Unhandled Message: prim %x len %d from addr %x, dinfo %x on this port.\n",frm->prim, frm->len, frm->addr, frm->dinfo);		
	free_msg(msg);
	

	return 0;
}




int misdn_lib_get_port_info(int port)
{
	msg_t *msg=alloc_msg(MAX_MSG_SIZE);
	iframe_t *frm;
	struct misdn_stack *stack=find_stack_by_port(port);
	if (!msg) {
		cb_log(-1, port, "misgn_lib_get_port: alloc_msg failed!\n");
		return -1;
	}
	frm=(iframe_t*)msg->data;
	if (!stack ) {
		cb_log(-1, port, "There is no Stack for this port.\n");
		return -1;
	}
	/* activate bchannel */
	frm->prim = CC_STATUS_ENQUIRY | REQUEST;

	frm->addr = stack->upper_id| FLG_MSG_DOWN;

	frm->dinfo = 0;
	frm->len = 0;
  
	msg_queue_tail(&glob_mgr->activatequeue, msg);
	sem_post(&glob_mgr->new_msg);

  
	return 0; 
}


int queue_cleanup_bc(struct misdn_bchannel *bc) 
{
	msg_t *msg=alloc_msg(MAX_MSG_SIZE);
	iframe_t *frm;
	if (!msg) {
		cb_log(-1, bc->port, "misgn_lib_get_port: alloc_msg failed!\n");
		return -1;
	}
	frm=(iframe_t*)msg->data;

	/* activate bchannel */
	frm->prim = MGR_CLEARSTACK| REQUEST;

	frm->addr = bc->l3_id;

	frm->dinfo = bc->port;
	frm->len = 0;
  
	msg_queue_tail(&glob_mgr->activatequeue, msg);
	sem_post(&glob_mgr->new_msg);

	return 0; 

}

int misdn_lib_port_restart(int port)
{
	struct misdn_stack *stack=find_stack_by_port(port);
 
	cb_log(0, port, "Restarting this port.\n");
	if (stack) {
		cb_log(0, port, "Stack:%p\n",stack);
		
		clear_l3(stack);
		{
			msg_t *msg=alloc_msg(MAX_MSG_SIZE);
			iframe_t *frm;

			if (!msg) {
				cb_log(-1, port, "port_restart: alloc_msg failed\n");
				return -1;
			}
			
			frm=(iframe_t*)msg->data;
			/* we must activate if we are deactivated */
			/* activate bchannel */
			frm->prim = DL_RELEASE | REQUEST;
			frm->addr = stack->upper_id | FLG_MSG_DOWN;

			frm->dinfo = 0;
			frm->len = 0;
			msg_queue_tail(&glob_mgr->activatequeue, msg);
			sem_post(&glob_mgr->new_msg);
		}
		return 0;
    
		stack_te_destroy(stack);
      
		{
			struct misdn_stack *tmpstack;
			struct misdn_stack *newstack=stack_init(stack->midev ,port, stack->ptp);
      
      
			if (stack == glob_mgr->stack_list) {
				struct misdn_stack *n=glob_mgr->stack_list->next;
				glob_mgr->stack_list = newstack ;
				glob_mgr->stack_list->next = n;
			} else {
				for (tmpstack=glob_mgr->stack_list;
				     tmpstack->next;
				     tmpstack=tmpstack->next) 
					if (tmpstack->next == stack) break;

				if (!tmpstack->next) {
					cb_log(-1, port, "Stack to restart not found\n");
					return 0;
				}  else {
					struct misdn_stack *n=tmpstack->next->next;
					tmpstack->next=newstack;
					newstack->next=n;
				}
			}
      
			{
				int i;
				for(i=0;i<newstack->b_num; i++) {
					int r;
					if ((r=init_bc(newstack, &newstack->bc[i], newstack->midev,port,i, "", 1))<0) {
						cb_log(-1, port, "Got Err @ init_bc :%d\n",r);
						return 0;
					}
				}
			}
      
			free(stack);
		}
	}

	return 0;
}



sem_t handler_started; 

void manager_event_handler(void *arg)
{
	sem_post(&handler_started); 
	while (1) {
		struct misdn_stack *stack;
		msg_t *msg;
    
		/** wait for events **/
		sem_wait(&glob_mgr->new_msg);
    
		for (msg=msg_dequeue(&glob_mgr->activatequeue);
		     msg;
		     msg=msg_dequeue(&glob_mgr->activatequeue)
			)
		{
	
			iframe_t *frm =  (iframe_t*) msg->data ;

			switch ( frm->prim) {

			case MGR_CLEARSTACK | REQUEST:
				/*a queued bchannel cleanup*/
				{
					struct misdn_stack *stack=find_stack_by_port(frm->dinfo);
					if (!stack) {
						cb_log(-1,0,"no stack found with port [%d]!! so we cannot cleanup the bc\n",frm->dinfo);
						free_msg(msg);
						break;
					}
					
					struct misdn_bchannel *bc=find_bc_by_l3id(stack,frm->addr);
					if (bc) {
						cb_log(1,bc->port,"CLEARSTACK queued, cleaning up\n");
						clean_up_bc(bc);
					} else {
						cb_log(-1,stack->port,"bc could not be cleaned correctly !! addr [%x]\n",frm->addr);
					}
				}
				free_msg(msg);	
				break;
			case MGR_SETSTACK | REQUEST :
				break;
			default:
				mISDN_write(glob_mgr->midev, frm, mISDN_HEADER_LEN+frm->len, TIMEOUT_1SEC);
				free_msg(msg);
			}
		}

		for (stack=glob_mgr->stack_list;
		     stack;
		     stack=stack->next ) { 

			while ( (msg=msg_dequeue(&stack->upqueue)) ) {
				/** Handle L2/3 Signalling after bchans **/ 
				if (!handle_frm_nt(msg)) {
					/* Maybe it's TE */
					if (!handle_frm(msg)) {
						/* wow none! */
						cb_log(-1,stack->port,"Wow we've got a strange issue while dequeueing a Frame\n");
					}
				}
			}

			/* Here we should check if we really want to 
				send all the messages we've queued, lets 
				assume we've queued a Disconnect, but 
				received it already from the other side!*/
		     
			while ( (msg=msg_dequeue(&stack->downqueue)) ) {
				if (stack->nt ) {
					if (stack->nst.manager_l3(&stack->nst, msg))
						cb_log(-1, stack->port, "Error@ Sending Message in NT-Stack.\n");
	  
				} else {
					iframe_t *frm = (iframe_t *)msg->data;
					struct misdn_bchannel *bc = find_bc_by_l3id(stack, frm->dinfo);
					cb_log(4,stack->port,"Sending msg, prim:%x addr:%x dinfo:%x\n",frm->prim,frm->addr,frm->dinfo);
					if (bc) send_msg(glob_mgr->midev, bc, msg);
				}
			}
		}
	}
}


int misdn_lib_maxports_get() { /** BE AWARE WE HAVE NO CB_LOG HERE! **/
	
	int i = mISDN_open();
	int max=0;
	
	if (i<0)
		return -1;

	max = mISDN_get_stack_count(i);
	
	mISDN_close(i);
	
	return max;
}

int misdn_lib_init(char *portlist, struct misdn_lib_iface *iface, void *user_data)
{
	struct misdn_lib *mgr=calloc(1, sizeof(struct misdn_lib));
	char *tok, *tokb;
	char plist[1024];
	int midev;
	int port_count=0;
 
	cb_log = iface->cb_log;
	cb_event = iface->cb_event;
	cb_jb_empty = iface->cb_jb_empty;
	
	glob_mgr = mgr;
  
	msg_init();
#if 0
	int flags=0xff;
	flags &= ~DBGM_MSG;
	debug_init( flags , NULL, NULL, NULL);
#else
	debug_init(0 , NULL, NULL, NULL);
#endif	
	if (!portlist || (*portlist == 0) ) return 1;
	
	init_flip_bits();
	
	{
		strncpy(plist,portlist, 1024);
		plist[1023] = 0;
	}
  
	memcpy(tone_425_flip,tone_425,TONE_425_SIZE);
	flip_buf_bits(tone_425_flip,TONE_425_SIZE);

	memcpy(tone_silence_flip,tone_SILENCE,TONE_SILENCE_SIZE);
	flip_buf_bits(tone_silence_flip,TONE_SILENCE_SIZE);
  
	midev=te_lib_init();
	mgr->midev=midev;

	port_count=mISDN_get_stack_count(midev);
  
	msg_queue_init(&mgr->activatequeue);
  
	if (sem_init(&mgr->new_msg, 1, 0)<0)
		sem_init(&mgr->new_msg, 0, 0);
  
	for (tok=strtok_r(plist," ,",&tokb );
	     tok; 
	     tok=strtok_r(NULL," ,",&tokb)) {
		int port = atoi(tok);
		struct misdn_stack *stack;
		static int first=1;
		int ptp=0;
    
		if (strstr(tok, "ptp"))
			ptp=1;

		if (port > port_count) {
			cb_log(-1, port, "Couldn't Initialize this port since we have only %d ports\n", port_count);
			exit(1);
		}
		stack=stack_init(midev, port, ptp);
    
		if (!stack) {
			perror("init_stack");
			exit(1);
		}
    
		if (stack && first) {
			mgr->stack_list=stack;
			first=0;
			{
				int i;
				for(i=0;i<stack->b_num; i++) {
					int r;
					if ((r=init_bc(stack, &stack->bc[i], stack->midev,port,i, "", 1))<0) {
						cb_log(-1, port, "Got Err @ init_bc :%d\n",r);
						exit(1);
					}
				}
			}
      
			continue;
		}
    
		if (stack) {
			struct misdn_stack * help;
			for ( help=mgr->stack_list; help; help=help->next ) 
				if (help->next == NULL) break;
      
      
			help->next=stack;

			{
				int i;
				for(i=0;i<stack->b_num; i++) {
					int r;
					if ((r=init_bc(stack, &stack->bc[i], stack->midev,port,i, "",1 ))<0) {
						cb_log(-1, port, "Got Err @ init_bc :%d\n",r);
						exit(1);
					} 
				}
			}
		}
    
	}
  
	if (sem_init(&handler_started, 1, 0)<0)
		sem_init(&handler_started, 0, 0);
  
	cb_log(4, 0, "Starting Event Handler\n");
	pthread_create( &mgr->event_handler_thread, NULL,(void*)manager_event_handler, mgr);
  
	sem_wait(&handler_started) ;
	cb_log(4, 0, "Starting Event Catcher\n");
	pthread_create( &mgr->event_thread, NULL, (void*)misdn_lib_isdn_event_catcher, mgr);
  
	cb_log(4, 0, "Event Catcher started\n");

	if (iface->l1watcher_timeout > 0) {
		mgr->l1watcher_timeout=iface->l1watcher_timeout;
		cb_log(4, 0, "Starting L1 watcher\n");
		pthread_create( &mgr->l1watcher_thread, NULL, (void*)misdn_lib_isdn_l1watcher, mgr);
	}
	
	global_state= MISDN_INITIALIZED; 
  
	return (mgr == NULL);
}

void misdn_lib_destroy()
{
	struct misdn_stack *help;
	int i;
  
	for ( help=glob_mgr->stack_list; help; help=help->next ) {
		for(i=0;i<help->b_num; i++) {
			char buf[1024];
			mISDN_write_frame(help->midev, buf, help->bc[i].addr, MGR_DELLAYER | REQUEST, 0, 0, NULL, TIMEOUT_1SEC);
			help->bc[i].addr = 0;
		}
		cb_log (1, help->port, "Destroying this port.\n");
		stack_te_destroy(help);
	}
	
	if (global_state == MISDN_INITIALIZED) {
		cb_log(4, 0, "Killing Handler Thread\n");
		if ( pthread_cancel(glob_mgr->event_handler_thread) == 0 ) {
			cb_log(4, 0, "Joining Handler Thread\n");
			pthread_join(glob_mgr->event_handler_thread, NULL);
		}
	  
		cb_log(4, 0, "Killing Main Thread\n");
		if ( pthread_cancel(glob_mgr->event_thread) == 0 ) {
			cb_log(4, 0, "Joining Main Thread\n");
			pthread_join(glob_mgr->event_thread, NULL);
		}
	}
  
	cb_log(1, 0, "Closing mISDN device\n");
	te_lib_destroy(glob_mgr->midev);
}

char *manager_isdn_get_info(enum event_e event)
{
	return isdn_get_info(msgs_g , event, 0);
}

void manager_bchannel_activate(struct misdn_bchannel *bc)
{
	char buf[128];
	iframe_t *ifrm;
	int ret;

	struct misdn_stack *stack=get_stack_by_bc(bc);

	if (!stack) {
		cb_log(-1, bc->port, "bchannel_activate: Stack not found !");
		return ;
	}
	
	/* we must activate if we are deactivated */
	clear_ibuffer(bc->astbuf);
	
	cb_log(5, stack->port, "$$$ Bchan Activated addr %x\n", bc->addr);
	
	mISDN_write_frame(stack->midev, buf, bc->addr | FLG_MSG_DOWN,  DL_ESTABLISH | REQUEST, 0,0, NULL, TIMEOUT_1SEC);

	ret=mISDN_read(stack->midev,buf,128,TIMEOUT_10SEC);

	ifrm=(iframe_t*)buf;
	
	if (ret>0) {
		if (ifrm->prim== (DL_ESTABLISH|CONFIRM)) {
			cb_log(2,stack->port,"bchan: DL_ESTABLISH|CNF\n");
		}
	}
	
	
	return ;
  
}


void manager_bchannel_deactivate(struct misdn_bchannel * bc)
{

	struct misdn_stack *stack=get_stack_by_bc(bc);


	switch (bc->bc_state) {
		case BCHAN_ACTIVATED:
			break;
		case BCHAN_BRIDGED:
			misdn_split_conf(bc,bc->conf_id);
			break;
		default:
			cb_log( 4, bc->port,"bchan_deactivate: called but not activated\n");
			return ;

	}
	
	cb_log(5, stack->port, "$$$ Bchan deActivated addr %x\n", bc->addr);
	
	bc->generate_tone=0;
	
	iframe_t dact;
	dact.prim = DL_RELEASE | REQUEST;
	dact.addr = bc->addr | FLG_MSG_DOWN;
	dact.dinfo = 0;
	dact.len = 0;
	char buf[128];	
	mISDN_write_frame(stack->midev, buf, bc->addr | FLG_MSG_DOWN, DL_RELEASE|REQUEST,0,0,NULL, TIMEOUT_1SEC);

	mISDN_read(stack->midev, buf, 128, TIMEOUT_1SEC);

	clear_ibuffer(bc->astbuf);
	
	bc_state_change(bc,BCHAN_RELEASE);
  
	return;
}


int misdn_lib_tx2misdn_frm(struct misdn_bchannel *bc, void *data, int len)
{
	struct misdn_stack *stack=get_stack_by_bc(bc);

	switch (bc->bc_state) {
		case BCHAN_ACTIVATED:
		case BCHAN_BRIDGED:
			break;
		default:
			cb_log(3, bc->port, "BC not yet activated (state:%s)\n",bc_state2str(bc->bc_state));
			return -1;
	}
	
	char buf[4096 + mISDN_HEADER_LEN];
	iframe_t *frm= (iframe_t*)buf;
	int  r;
	
	frm->prim = DL_DATA|REQUEST;
	frm->dinfo = 0;
	frm->addr = bc->addr | FLG_MSG_DOWN ;
	
	frm->len = len;
	memcpy(&buf[mISDN_HEADER_LEN], data,len);
		
	if ( misdn_cap_is_speech(bc->capability) ) 
		flip_buf_bits( &buf[mISDN_HEADER_LEN], len);
	else
		cb_log(6, stack->port, "Writing %d data bytes\n",len);
	
	cb_log(9, stack->port, "Writing %d bytes 2 mISDN\n",len);
	r=mISDN_write(stack->midev, buf, frm->len + mISDN_HEADER_LEN, TIMEOUT_INFINIT);
#ifdef ACK_HDLC
	if (bc->hdlc && bc->ack_hdlc) {
		cb_log(4,stack->port,"Awaiting Acknowledge [%d]\n",len);
		sem_wait((sem_t*)bc->ack_hdlc);
		cb_log(4,stack->port,"Acknowledged\n");
	}
#endif	
	return 0;
}



/*
 * send control information to the channel (dsp-module)
 */
void manager_ph_control(struct misdn_bchannel *bc, int c1, int c2)
{
	unsigned char buffer[mISDN_HEADER_LEN+2*sizeof(int)];
	iframe_t *ctrl = (iframe_t *)buffer; /* preload data */
	unsigned int *d = (unsigned int*)&ctrl->data.p;
	struct misdn_stack *stack=get_stack_by_bc(bc);
	
	cb_log(4,bc->port,"ph_control: c1:%x c2:%x\n",c1,c2);
	
	ctrl->prim = PH_CONTROL | REQUEST;
	ctrl->addr = bc->addr | FLG_MSG_DOWN;
	ctrl->dinfo = 0;
	ctrl->len = sizeof(unsigned int)*2;
	*d++ = c1;
	*d++ = c2;
	mISDN_write(stack->midev, ctrl, mISDN_HEADER_LEN+ctrl->len, TIMEOUT_1SEC);
}

/*
 * send control information to the channel (dsp-module)
 */
void manager_ph_control_block(struct misdn_bchannel *bc, int c1, void *c2, int c2_len)
{
	unsigned char buffer[mISDN_HEADER_LEN+sizeof(int)+c2_len];
	iframe_t *ctrl = (iframe_t *)buffer;
	unsigned int *d = (unsigned int *)&ctrl->data.p;
	struct misdn_stack *stack=get_stack_by_bc(bc);
	
	ctrl->prim = PH_CONTROL | REQUEST;
	ctrl->addr = bc->addr | FLG_MSG_DOWN;
	ctrl->dinfo = 0;
	ctrl->len = sizeof(unsigned int) + c2_len;
	*d++ = c1;
	memcpy(d, c2, c2_len);
	mISDN_write(stack->midev, ctrl, mISDN_HEADER_LEN+ctrl->len, TIMEOUT_1SEC);
}




void manager_clean_bc(struct misdn_bchannel *bc )
{
	struct misdn_stack *stack=get_stack_by_bc(bc);
	
	empty_chan_in_stack(stack, bc->channel);
	empty_bc(bc);
  
	misdn_lib_send_event(bc,EVENT_RELEASE_COMPLETE);
}


void stack_holder_add(struct misdn_stack *stack, struct misdn_bchannel *holder)
{
	struct misdn_bchannel *help;
	cb_log(4,stack->port, "*HOLDER: add %x\n",holder->l3_id);
	
	holder->stack_holder=1;

	if (!stack ) return ;
	
	holder->next=NULL;
	
	if (!stack->holding) {
		stack->holding = holder;
		return;
	}
  
	for (help=stack->holding;
	     help;
	     help=help->next) {
		if (!help->next) {
			help->next=holder;
			break;
		}
	}
  
}

void stack_holder_remove(struct misdn_stack *stack, struct misdn_bchannel *holder)
{
	struct misdn_bchannel *h1;

	if (!holder->stack_holder) return;
	
	cb_log(4,stack->port, "*HOLDER: remove %x\n",holder->l3_id);
	if (!stack || ! stack->holding) return;
  
	if (holder == stack->holding) {
		stack->holding = stack->holding->next;
		return;
	}
	
	for (h1=stack->holding;
	     h1;
	     h1=h1->next) {
		if (h1->next == holder) {
			h1->next=h1->next->next;
			return ;
		}
	}
}


struct misdn_bchannel *stack_holder_find(struct misdn_stack *stack, unsigned long l3id)
{
	struct misdn_bchannel *help;

	cb_log(4,stack?stack->port:0, "*HOLDER: find %x\n",l3id);
	
	if (!stack) return NULL;
	
	for (help=stack->holding;
	     help;
	     help=help->next) {
		if (help->l3_id == l3id) {
			cb_log(4,stack->port, "*HOLDER: found bc\n");
			return help;
		}
	}

	cb_log(4,stack->port, "*HOLDER: find nothing\n");
	return NULL;
}



void misdn_lib_send_tone(struct misdn_bchannel *bc, enum tone_e tone) 
{

	switch(tone) {
	case TONE_DIAL:
		manager_ph_control(bc, TONE_PATT_ON, TONE_GERMAN_DIALTONE);	
	break;
	
	case TONE_ALERTING:
		manager_ph_control(bc, TONE_PATT_ON, TONE_GERMAN_RINGING);	
	break;
	
	case TONE_HANGUP:
		manager_ph_control(bc, TONE_PATT_ON, TONE_GERMAN_HANGUP);	
	break;

	case TONE_NONE:
	default:
		manager_ph_control(bc, TONE_PATT_OFF, TONE_GERMAN_HANGUP);	
	}

	char buf[mISDN_HEADER_LEN+128];
	iframe_t *frm=(iframe_t*)buf;
	memset(buf,0,mISDN_HEADER_LEN+128);

	frm->prim=DL_DATA|REQUEST;
	frm->addr=bc->addr|FLG_MSG_DOWN;
	frm->dinfo=0;
	frm->len=128;
	
	mISDN_write(glob_mgr->midev, frm, mISDN_HEADER_LEN+frm->len, TIMEOUT_1SEC);
}


void manager_ec_enable(struct misdn_bchannel *bc)
{
	int ec_arr[2];

	struct misdn_stack *stack=get_stack_by_bc(bc);
	
	cb_log(1, stack?stack->port:0,"Sending Control ECHOCAN_ON taps:%d training:%d\n",bc->ec_deftaps, bc->ec_training);
	
	switch (bc->ec_deftaps) {
	case 4:
	case 8:
	case 16:
	case 32:
	case 64:
	case 128:
	case 256:
	case 512:
	case 1024:
		cb_log(4, stack->port, "Taps is %d\n",bc->ec_deftaps);
		break;
	default:
		cb_log(-1, stack->port, "Taps should be power of 2\n");
		bc->ec_deftaps=128;
	}

	ec_arr[0]=bc->ec_deftaps;
	ec_arr[1]=bc->ec_training;
	
	manager_ph_control_block(bc,  ECHOCAN_ON,  ec_arr, sizeof(ec_arr));
}



void manager_ec_disable(struct misdn_bchannel *bc)
{
	struct misdn_stack *stack=get_stack_by_bc(bc);
	
	cb_log(1, stack?stack->port:0, "Sending Control ECHOCAN_OFF\n");
	manager_ph_control(bc,  ECHOCAN_OFF, 0);
}

struct misdn_stack* get_misdn_stack() {
	return glob_mgr->stack_list;
}



void misdn_join_conf(struct misdn_bchannel *bc, int conf_id)
{
	bc_state_change(bc,BCHAN_BRIDGED);
	manager_ph_control(bc, CMX_RECEIVE_OFF, 0);
	manager_ph_control(bc, CMX_CONF_JOIN, conf_id);

	cb_log(1,bc->port, "Joining bc:%x in conf:%d\n",bc->addr,conf_id);

	char data[16];
	int len=15;

	memset(data,0,15);
	
	misdn_lib_tx2misdn_frm(bc, data, len);

}


void misdn_split_conf(struct misdn_bchannel *bc, int conf_id)
{
	bc_state_change(bc,BCHAN_ACTIVATED);
	manager_ph_control(bc, CMX_RECEIVE_ON, 0);
	manager_ph_control(bc, CMX_CONF_SPLIT, conf_id);

	cb_log(1,bc->port, "Splitting bc:%x in conf:%d\n",bc->addr,conf_id);
}

void misdn_lib_bridge( struct misdn_bchannel * bc1, struct misdn_bchannel *bc2) {
	int conf_id=bc1->pid +1;

	cb_log(1, bc1->port, "I Send: BRIDGE from:%d to:%d\n",bc1->port,bc2->port);
	
	struct misdn_bchannel *bc_list[]={
		bc1,bc2,NULL
	};
	struct misdn_bchannel **bc;
		
	for (bc=bc_list; *bc;  *bc++) { 
		(*bc)->conf_id=conf_id;
		cb_log(1, (*bc)->port, " --> bc_addr:%x\n",(*bc)->addr);
	
		switch((*bc)->bc_state) {
			case BCHAN_ACTIVATED:
				misdn_join_conf(*bc,conf_id);
				break;
			default:
				bc_next_state_change(*bc,BCHAN_BRIDGED);
				break;
		}
	}
}

void misdn_lib_split_bridge( struct misdn_bchannel * bc1, struct misdn_bchannel *bc2)
{

	struct misdn_bchannel *bc_list[]={
		bc1,bc2,NULL
	};
	struct misdn_bchannel **bc;
		
	for (bc=bc_list; *bc;  *bc++) { 
		if ( (*bc)->bc_state == BCHAN_BRIDGED){
			misdn_split_conf( *bc, (*bc)->conf_id);
		} else {
			cb_log( 2, (*bc)->port, "BC not bridged (state:%s) so not splitting it\n",bc_state2str((*bc)->bc_state));
		}
	}
	
}



void misdn_lib_echo(struct misdn_bchannel *bc, int onoff)
{
	cb_log(1,bc->port, " --> ECHO %s\n", onoff?"ON":"OFF");
	manager_ph_control(bc, onoff?CMX_ECHO_ON:CMX_ECHO_OFF, 0);
}



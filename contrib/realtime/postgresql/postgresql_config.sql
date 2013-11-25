BEGIN;

CREATE TABLE alembic_version (
    version_num VARCHAR(32) NOT NULL
);

-- Running upgrade None -> 4da0c5f79a9c

CREATE TYPE type_values AS ENUM ('friend','user','peer');

CREATE TYPE sip_transport_values AS ENUM ('udp','tcp','tls','ws','wss','udp,tcp','tcp,udp');

CREATE TYPE sip_dtmfmode_values AS ENUM ('rfc2833','info','shortinfo','inband','auto');

CREATE TYPE sip_directmedia_values AS ENUM ('yes','no','nonat','update');

CREATE TYPE yes_no_values AS ENUM ('yes','no');

CREATE TYPE sip_progressinband_values AS ENUM ('yes','no','never');

CREATE TYPE sip_session_timers_values AS ENUM ('accept','refuse','originate');

CREATE TYPE sip_session_refresher_values AS ENUM ('uac','uas');

CREATE TYPE sip_callingpres_values AS ENUM ('allowed_not_screened','allowed_passed_screen','allowed_failed_screen','allowed','prohib_not_screened','prohib_passed_screen','prohib_failed_screen','prohib');

CREATE TABLE sippeers (
    id SERIAL NOT NULL, 
    name VARCHAR(40) NOT NULL, 
    ipaddr VARCHAR(45), 
    port INTEGER, 
    regseconds INTEGER, 
    defaultuser VARCHAR(40), 
    fullcontact VARCHAR(80), 
    regserver VARCHAR(20), 
    useragent VARCHAR(20), 
    lastms INTEGER, 
    host VARCHAR(40), 
    type type_values, 
    context VARCHAR(40), 
    permit VARCHAR(95), 
    deny VARCHAR(95), 
    secret VARCHAR(40), 
    md5secret VARCHAR(40), 
    remotesecret VARCHAR(40), 
    transport sip_transport_values, 
    dtmfmode sip_dtmfmode_values, 
    directmedia sip_directmedia_values, 
    nat VARCHAR(29), 
    callgroup VARCHAR(40), 
    pickupgroup VARCHAR(40), 
    language VARCHAR(40), 
    disallow VARCHAR(200), 
    allow VARCHAR(200), 
    insecure VARCHAR(40), 
    trustrpid yes_no_values, 
    progressinband sip_progressinband_values, 
    promiscredir yes_no_values, 
    useclientcode yes_no_values, 
    accountcode VARCHAR(40), 
    setvar VARCHAR(200), 
    callerid VARCHAR(40), 
    amaflags VARCHAR(40), 
    callcounter yes_no_values, 
    busylevel INTEGER, 
    allowoverlap yes_no_values, 
    allowsubscribe yes_no_values, 
    videosupport yes_no_values, 
    maxcallbitrate INTEGER, 
    rfc2833compensate yes_no_values, 
    mailbox VARCHAR(40), 
    "session-timers" sip_session_timers_values, 
    "session-expires" INTEGER, 
    "session-minse" INTEGER, 
    "session-refresher" sip_session_refresher_values, 
    t38pt_usertpsource VARCHAR(40), 
    regexten VARCHAR(40), 
    fromdomain VARCHAR(40), 
    fromuser VARCHAR(40), 
    qualify VARCHAR(40), 
    defaultip VARCHAR(45), 
    rtptimeout INTEGER, 
    rtpholdtimeout INTEGER, 
    sendrpid yes_no_values, 
    outboundproxy VARCHAR(40), 
    callbackextension VARCHAR(40), 
    timert1 INTEGER, 
    timerb INTEGER, 
    qualifyfreq INTEGER, 
    constantssrc yes_no_values, 
    contactpermit VARCHAR(95), 
    contactdeny VARCHAR(95), 
    usereqphone yes_no_values, 
    textsupport yes_no_values, 
    faxdetect yes_no_values, 
    buggymwi yes_no_values, 
    auth VARCHAR(40), 
    fullname VARCHAR(40), 
    trunkname VARCHAR(40), 
    cid_number VARCHAR(40), 
    callingpres sip_callingpres_values, 
    mohinterpret VARCHAR(40), 
    mohsuggest VARCHAR(40), 
    parkinglot VARCHAR(40), 
    hasvoicemail yes_no_values, 
    subscribemwi yes_no_values, 
    vmexten VARCHAR(40), 
    autoframing yes_no_values, 
    rtpkeepalive INTEGER, 
    "call-limit" INTEGER, 
    g726nonstandard yes_no_values, 
    ignoresdpversion yes_no_values, 
    allowtransfer yes_no_values, 
    dynamic yes_no_values, 
    path VARCHAR(256), 
    supportpath yes_no_values, 
    PRIMARY KEY (id), 
    UNIQUE (name)
);

CREATE INDEX sippeers_name ON sippeers (name);

CREATE INDEX sippeers_name_host ON sippeers (name, host);

CREATE INDEX sippeers_ipaddr_port ON sippeers (ipaddr, port);

CREATE INDEX sippeers_host_port ON sippeers (host, port);

CREATE TYPE iax_requirecalltoken_values AS ENUM ('yes','no','auto');

CREATE TYPE iax_encryption_values AS ENUM ('yes','no','aes128');

CREATE TYPE iax_transfer_values AS ENUM ('yes','no','mediaonly');

CREATE TABLE iaxfriends (
    id SERIAL NOT NULL, 
    name VARCHAR(40) NOT NULL, 
    type type_values, 
    username VARCHAR(40), 
    mailbox VARCHAR(40), 
    secret VARCHAR(40), 
    dbsecret VARCHAR(40), 
    context VARCHAR(40), 
    regcontext VARCHAR(40), 
    host VARCHAR(40), 
    ipaddr VARCHAR(40), 
    port INTEGER, 
    defaultip VARCHAR(20), 
    sourceaddress VARCHAR(20), 
    mask VARCHAR(20), 
    regexten VARCHAR(40), 
    regseconds INTEGER, 
    accountcode VARCHAR(20), 
    mohinterpret VARCHAR(20), 
    mohsuggest VARCHAR(20), 
    inkeys VARCHAR(40), 
    outkeys VARCHAR(40), 
    language VARCHAR(10), 
    callerid VARCHAR(100), 
    cid_number VARCHAR(40), 
    sendani yes_no_values, 
    fullname VARCHAR(40), 
    trunk yes_no_values, 
    auth VARCHAR(20), 
    maxauthreq INTEGER, 
    requirecalltoken iax_requirecalltoken_values, 
    encryption iax_encryption_values, 
    transfer iax_transfer_values, 
    jitterbuffer yes_no_values, 
    forcejitterbuffer yes_no_values, 
    disallow VARCHAR(200), 
    allow VARCHAR(200), 
    codecpriority VARCHAR(40), 
    qualify VARCHAR(10), 
    qualifysmoothing yes_no_values, 
    qualifyfreqok VARCHAR(10), 
    qualifyfreqnotok VARCHAR(10), 
    timezone VARCHAR(20), 
    adsi yes_no_values, 
    amaflags VARCHAR(20), 
    setvar VARCHAR(200), 
    PRIMARY KEY (id), 
    UNIQUE (name)
);

CREATE INDEX iaxfriends_name ON iaxfriends (name);

CREATE INDEX iaxfriends_name_host ON iaxfriends (name, host);

CREATE INDEX iaxfriends_name_ipaddr_port ON iaxfriends (name, ipaddr, port);

CREATE INDEX iaxfriends_ipaddr_port ON iaxfriends (ipaddr, port);

CREATE INDEX iaxfriends_host_port ON iaxfriends (host, port);

CREATE TABLE voicemail (
    uniqueid SERIAL NOT NULL, 
    context VARCHAR(80) NOT NULL, 
    mailbox VARCHAR(80) NOT NULL, 
    password VARCHAR(80) NOT NULL, 
    fullname VARCHAR(80), 
    alias VARCHAR(80), 
    email VARCHAR(80), 
    pager VARCHAR(80), 
    attach yes_no_values, 
    attachfmt VARCHAR(10), 
    serveremail VARCHAR(80), 
    language VARCHAR(20), 
    tz VARCHAR(30), 
    deletevoicemail yes_no_values, 
    saycid yes_no_values, 
    sendvoicemail yes_no_values, 
    review yes_no_values, 
    tempgreetwarn yes_no_values, 
    operator yes_no_values, 
    envelope yes_no_values, 
    sayduration INTEGER, 
    forcename yes_no_values, 
    forcegreetings yes_no_values, 
    callback VARCHAR(80), 
    dialout VARCHAR(80), 
    exitcontext VARCHAR(80), 
    maxmsg INTEGER, 
    volgain NUMERIC(5, 2), 
    imapuser VARCHAR(80), 
    imappassword VARCHAR(80), 
    imapserver VARCHAR(80), 
    imapport VARCHAR(8), 
    imapflags VARCHAR(80), 
    stamp TIMESTAMP WITHOUT TIME ZONE, 
    PRIMARY KEY (uniqueid)
);

CREATE INDEX voicemail_mailbox ON voicemail (mailbox);

CREATE INDEX voicemail_context ON voicemail (context);

CREATE INDEX voicemail_mailbox_context ON voicemail (mailbox, context);

CREATE INDEX voicemail_imapuser ON voicemail (imapuser);

CREATE TABLE meetme (
    bookid SERIAL NOT NULL, 
    confno VARCHAR(80) NOT NULL, 
    starttime TIMESTAMP WITHOUT TIME ZONE, 
    endtime TIMESTAMP WITHOUT TIME ZONE, 
    pin VARCHAR(20), 
    adminpin VARCHAR(20), 
    opts VARCHAR(20), 
    adminopts VARCHAR(20), 
    recordingfilename VARCHAR(80), 
    recordingformat VARCHAR(10), 
    maxusers INTEGER, 
    members INTEGER NOT NULL, 
    PRIMARY KEY (bookid)
);

CREATE INDEX meetme_confno_start_end ON meetme (confno, starttime, endtime);

CREATE TYPE moh_mode_values AS ENUM ('custom','files','mp3nb','quietmp3nb','quietmp3');

CREATE TABLE musiconhold (
    name VARCHAR(80) NOT NULL, 
    mode moh_mode_values, 
    directory VARCHAR(255), 
    application VARCHAR(255), 
    digit VARCHAR(1), 
    sort VARCHAR(10), 
    format VARCHAR(10), 
    stamp TIMESTAMP WITHOUT TIME ZONE, 
    PRIMARY KEY (name)
);

-- Running upgrade 4da0c5f79a9c -> 43956d550a44

CREATE TYPE yesno_values AS ENUM ('yes','no');

CREATE TYPE pjsip_connected_line_method_values AS ENUM ('invite','reinvite','update');

CREATE TYPE pjsip_direct_media_glare_mitigation_values AS ENUM ('none','outgoing','incoming');

CREATE TYPE pjsip_dtmf_mode_values AS ENUM ('rfc4733','inband','info');

CREATE TYPE pjsip_identify_by_values AS ENUM ('username');

CREATE TYPE pjsip_timer_values AS ENUM ('forced','no','required','yes');

CREATE TYPE pjsip_cid_privacy_values AS ENUM ('allowed_not_screened','allowed_passed_screened','allowed_failed_screened','allowed','prohib_not_screened','prohib_passed_screened','prohib_failed_screened','prohib','unavailable');

CREATE TYPE pjsip_100rel_values AS ENUM ('no','required','yes');

CREATE TYPE pjsip_media_encryption_values AS ENUM ('no','sdes','dtls');

CREATE TYPE pjsip_t38udptl_ec_values AS ENUM ('none','fec','redundancy');

CREATE TYPE pjsip_dtls_setup_values AS ENUM ('active','passive','actpass');

CREATE TABLE ps_endpoints (
    id VARCHAR(40) NOT NULL, 
    transport VARCHAR(40), 
    aors VARCHAR(200), 
    auth VARCHAR(40), 
    context VARCHAR(40), 
    disallow VARCHAR(200), 
    allow VARCHAR(200), 
    direct_media yesno_values, 
    connected_line_method pjsip_connected_line_method_values, 
    direct_media_method pjsip_connected_line_method_values, 
    direct_media_glare_mitigation pjsip_direct_media_glare_mitigation_values, 
    disable_direct_media_on_nat yesno_values, 
    dtmf_mode pjsip_dtmf_mode_values, 
    external_media_address VARCHAR(40), 
    force_rport yesno_values, 
    ice_support yesno_values, 
    identify_by pjsip_identify_by_values, 
    mailboxes VARCHAR(40), 
    moh_suggest VARCHAR(40), 
    outbound_auth VARCHAR(40), 
    outbound_proxy VARCHAR(40), 
    rewrite_contact yesno_values, 
    rtp_ipv6 yesno_values, 
    rtp_symmetric yesno_values, 
    send_diversion yesno_values, 
    send_pai yesno_values, 
    send_rpid yesno_values, 
    timers_min_se INTEGER, 
    timers pjsip_timer_values, 
    timers_sess_expires INTEGER, 
    callerid VARCHAR(40), 
    callerid_privacy pjsip_cid_privacy_values, 
    callerid_tag VARCHAR(40), 
    "100rel" pjsip_100rel_values, 
    aggregate_mwi yesno_values, 
    trust_id_inbound yesno_values, 
    trust_id_outbound yesno_values, 
    use_ptime yesno_values, 
    use_avpf yesno_values, 
    media_encryption pjsip_media_encryption_values, 
    inband_progress yesno_values, 
    call_group VARCHAR(40), 
    pickup_group VARCHAR(40), 
    named_call_group VARCHAR(40), 
    named_pickup_group VARCHAR(40), 
    device_state_busy_at INTEGER, 
    fax_detect yesno_values, 
    t38_udptl yesno_values, 
    t38_udptl_ec pjsip_t38udptl_ec_values, 
    t38_udptl_maxdatagram INTEGER, 
    t38_udptl_nat yesno_values, 
    t38_udptl_ipv6 yesno_values, 
    tone_zone VARCHAR(40), 
    language VARCHAR(40), 
    one_touch_recording yesno_values, 
    record_on_feature VARCHAR(40), 
    record_off_feature VARCHAR(40), 
    rtp_engine VARCHAR(40), 
    allow_transfer yesno_values, 
    allow_subscribe yesno_values, 
    sdp_owner VARCHAR(40), 
    sdp_session VARCHAR(40), 
    tos_audio INTEGER, 
    tos_video INTEGER, 
    cos_audio INTEGER, 
    cos_video INTEGER, 
    sub_min_expiry INTEGER, 
    from_domain VARCHAR(40), 
    from_user VARCHAR(40), 
    mwi_fromuser VARCHAR(40), 
    dtls_verify VARCHAR(40), 
    dtls_rekey VARCHAR(40), 
    dtls_cert_file VARCHAR(200), 
    dtls_private_key VARCHAR(200), 
    dtls_cipher VARCHAR(200), 
    dtls_ca_file VARCHAR(200), 
    dtls_ca_path VARCHAR(200), 
    dtls_setup pjsip_dtls_setup_values, 
    srtp_tag_32 yesno_values, 
    UNIQUE (id)
);

CREATE INDEX ps_endpoints_id ON ps_endpoints (id);

CREATE TYPE pjsip_auth_type_values AS ENUM ('md5','userpass');

CREATE TABLE ps_auths (
    id VARCHAR(40) NOT NULL, 
    auth_type pjsip_auth_type_values, 
    nonce_lifetime INTEGER, 
    md5_cred VARCHAR(40), 
    password VARCHAR(80), 
    realm VARCHAR(40), 
    username VARCHAR(40), 
    UNIQUE (id)
);

CREATE INDEX ps_auths_id ON ps_auths (id);

CREATE TABLE ps_aors (
    id VARCHAR(40) NOT NULL, 
    contact VARCHAR(40), 
    default_expiration INTEGER, 
    mailboxes VARCHAR(80), 
    max_contacts INTEGER, 
    minimum_expiration INTEGER, 
    remove_existing yesno_values, 
    qualify_frequency INTEGER, 
    authenticate_qualify yesno_values, 
    UNIQUE (id)
);

CREATE INDEX ps_aors_id ON ps_aors (id);

CREATE TABLE ps_contacts (
    id VARCHAR(40) NOT NULL, 
    uri VARCHAR(40), 
    expiration_time VARCHAR(40), 
    qualify_frequency INTEGER, 
    UNIQUE (id)
);

CREATE INDEX ps_contacts_id ON ps_contacts (id);

CREATE TABLE ps_domain_aliases (
    id VARCHAR(40) NOT NULL, 
    domain VARCHAR(80), 
    UNIQUE (id)
);

CREATE INDEX ps_domain_aliases_id ON ps_domain_aliases (id);

CREATE TABLE ps_endpoint_id_ips (
    id VARCHAR(40) NOT NULL, 
    endpoint VARCHAR(40), 
    match VARCHAR(80), 
    UNIQUE (id)
);

CREATE INDEX ps_endpoint_id_ips_id ON ps_endpoint_id_ips (id);

INSERT INTO alembic_version (version_num) VALUES ('43956d550a44');

COMMIT;


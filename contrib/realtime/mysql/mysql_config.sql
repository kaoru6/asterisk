CREATE TABLE alembic_version (
    version_num VARCHAR(32) NOT NULL
);

-- Running upgrade None -> 4da0c5f79a9c

CREATE TABLE sippeers (
    id INTEGER NOT NULL AUTO_INCREMENT, 
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
    type ENUM('friend','user','peer'), 
    context VARCHAR(40), 
    permit VARCHAR(95), 
    deny VARCHAR(95), 
    secret VARCHAR(40), 
    md5secret VARCHAR(40), 
    remotesecret VARCHAR(40), 
    transport ENUM('udp','tcp','tls','ws','wss','udp,tcp','tcp,udp'), 
    dtmfmode ENUM('rfc2833','info','shortinfo','inband','auto'), 
    directmedia ENUM('yes','no','nonat','update'), 
    nat VARCHAR(29), 
    callgroup VARCHAR(40), 
    pickupgroup VARCHAR(40), 
    language VARCHAR(40), 
    disallow VARCHAR(200), 
    allow VARCHAR(200), 
    insecure VARCHAR(40), 
    trustrpid ENUM('yes','no'), 
    progressinband ENUM('yes','no','never'), 
    promiscredir ENUM('yes','no'), 
    useclientcode ENUM('yes','no'), 
    accountcode VARCHAR(40), 
    setvar VARCHAR(200), 
    callerid VARCHAR(40), 
    amaflags VARCHAR(40), 
    callcounter ENUM('yes','no'), 
    busylevel INTEGER, 
    allowoverlap ENUM('yes','no'), 
    allowsubscribe ENUM('yes','no'), 
    videosupport ENUM('yes','no'), 
    maxcallbitrate INTEGER, 
    rfc2833compensate ENUM('yes','no'), 
    mailbox VARCHAR(40), 
    `session-timers` ENUM('accept','refuse','originate'), 
    `session-expires` INTEGER, 
    `session-minse` INTEGER, 
    `session-refresher` ENUM('uac','uas'), 
    t38pt_usertpsource VARCHAR(40), 
    regexten VARCHAR(40), 
    fromdomain VARCHAR(40), 
    fromuser VARCHAR(40), 
    qualify VARCHAR(40), 
    defaultip VARCHAR(45), 
    rtptimeout INTEGER, 
    rtpholdtimeout INTEGER, 
    sendrpid ENUM('yes','no'), 
    outboundproxy VARCHAR(40), 
    callbackextension VARCHAR(40), 
    timert1 INTEGER, 
    timerb INTEGER, 
    qualifyfreq INTEGER, 
    constantssrc ENUM('yes','no'), 
    contactpermit VARCHAR(95), 
    contactdeny VARCHAR(95), 
    usereqphone ENUM('yes','no'), 
    textsupport ENUM('yes','no'), 
    faxdetect ENUM('yes','no'), 
    buggymwi ENUM('yes','no'), 
    auth VARCHAR(40), 
    fullname VARCHAR(40), 
    trunkname VARCHAR(40), 
    cid_number VARCHAR(40), 
    callingpres ENUM('allowed_not_screened','allowed_passed_screen','allowed_failed_screen','allowed','prohib_not_screened','prohib_passed_screen','prohib_failed_screen','prohib'), 
    mohinterpret VARCHAR(40), 
    mohsuggest VARCHAR(40), 
    parkinglot VARCHAR(40), 
    hasvoicemail ENUM('yes','no'), 
    subscribemwi ENUM('yes','no'), 
    vmexten VARCHAR(40), 
    autoframing ENUM('yes','no'), 
    rtpkeepalive INTEGER, 
    `call-limit` INTEGER, 
    g726nonstandard ENUM('yes','no'), 
    ignoresdpversion ENUM('yes','no'), 
    allowtransfer ENUM('yes','no'), 
    dynamic ENUM('yes','no'), 
    path VARCHAR(256), 
    supportpath ENUM('yes','no'), 
    PRIMARY KEY (id), 
    UNIQUE (name)
);

CREATE INDEX sippeers_name ON sippeers (name);

CREATE INDEX sippeers_name_host ON sippeers (name, host);

CREATE INDEX sippeers_ipaddr_port ON sippeers (ipaddr, port);

CREATE INDEX sippeers_host_port ON sippeers (host, port);

CREATE TABLE iaxfriends (
    id INTEGER NOT NULL AUTO_INCREMENT, 
    name VARCHAR(40) NOT NULL, 
    type ENUM('friend','user','peer'), 
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
    sendani ENUM('yes','no'), 
    fullname VARCHAR(40), 
    trunk ENUM('yes','no'), 
    auth VARCHAR(20), 
    maxauthreq INTEGER, 
    requirecalltoken ENUM('yes','no','auto'), 
    encryption ENUM('yes','no','aes128'), 
    transfer ENUM('yes','no','mediaonly'), 
    jitterbuffer ENUM('yes','no'), 
    forcejitterbuffer ENUM('yes','no'), 
    disallow VARCHAR(200), 
    allow VARCHAR(200), 
    codecpriority VARCHAR(40), 
    qualify VARCHAR(10), 
    qualifysmoothing ENUM('yes','no'), 
    qualifyfreqok VARCHAR(10), 
    qualifyfreqnotok VARCHAR(10), 
    timezone VARCHAR(20), 
    adsi ENUM('yes','no'), 
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
    uniqueid INTEGER NOT NULL AUTO_INCREMENT, 
    context VARCHAR(80) NOT NULL, 
    mailbox VARCHAR(80) NOT NULL, 
    password VARCHAR(80) NOT NULL, 
    fullname VARCHAR(80), 
    alias VARCHAR(80), 
    email VARCHAR(80), 
    pager VARCHAR(80), 
    attach ENUM('yes','no'), 
    attachfmt VARCHAR(10), 
    serveremail VARCHAR(80), 
    language VARCHAR(20), 
    tz VARCHAR(30), 
    deletevoicemail ENUM('yes','no'), 
    saycid ENUM('yes','no'), 
    sendvoicemail ENUM('yes','no'), 
    review ENUM('yes','no'), 
    tempgreetwarn ENUM('yes','no'), 
    operator ENUM('yes','no'), 
    envelope ENUM('yes','no'), 
    sayduration INTEGER, 
    forcename ENUM('yes','no'), 
    forcegreetings ENUM('yes','no'), 
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
    stamp DATETIME, 
    PRIMARY KEY (uniqueid)
);

CREATE INDEX voicemail_mailbox ON voicemail (mailbox);

CREATE INDEX voicemail_context ON voicemail (context);

CREATE INDEX voicemail_mailbox_context ON voicemail (mailbox, context);

CREATE INDEX voicemail_imapuser ON voicemail (imapuser);

CREATE TABLE meetme (
    bookid INTEGER NOT NULL AUTO_INCREMENT, 
    confno VARCHAR(80) NOT NULL, 
    starttime DATETIME, 
    endtime DATETIME, 
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

CREATE TABLE musiconhold (
    name VARCHAR(80) NOT NULL, 
    mode ENUM('custom','files','mp3nb','quietmp3nb','quietmp3'), 
    directory VARCHAR(255), 
    application VARCHAR(255), 
    digit VARCHAR(1), 
    sort VARCHAR(10), 
    format VARCHAR(10), 
    stamp DATETIME, 
    PRIMARY KEY (name)
);

INSERT INTO alembic_version (version_num) VALUES ('4da0c5f79a9c');

-- Running upgrade 4da0c5f79a9c -> 43956d550a44

CREATE TABLE ps_endpoints (
    id VARCHAR(40) NOT NULL, 
    transport VARCHAR(40), 
    aors VARCHAR(200), 
    auth VARCHAR(40), 
    context VARCHAR(40), 
    disallow VARCHAR(200), 
    allow VARCHAR(200), 
    direct_media ENUM('yes','no'), 
    connected_line_method ENUM('invite','reinvite','update'), 
    direct_media_method ENUM('invite','reinvite','update'), 
    direct_media_glare_mitigation ENUM('none','outgoing','incoming'), 
    disable_direct_media_on_nat ENUM('yes','no'), 
    dtmf_mode ENUM('rfc4733','inband','info'), 
    external_media_address VARCHAR(40), 
    force_rport ENUM('yes','no'), 
    ice_support ENUM('yes','no'), 
    identify_by ENUM('username'), 
    mailboxes VARCHAR(40), 
    moh_suggest VARCHAR(40), 
    outbound_auth VARCHAR(40), 
    outbound_proxy VARCHAR(40), 
    rewrite_contact ENUM('yes','no'), 
    rtp_ipv6 ENUM('yes','no'), 
    rtp_symmetric ENUM('yes','no'), 
    send_diversion ENUM('yes','no'), 
    send_pai ENUM('yes','no'), 
    send_rpid ENUM('yes','no'), 
    timers_min_se INTEGER, 
    timers ENUM('forced','no','required','yes'), 
    timers_sess_expires INTEGER, 
    callerid VARCHAR(40), 
    callerid_privacy ENUM('allowed_not_screened','allowed_passed_screened','allowed_failed_screened','allowed','prohib_not_screened','prohib_passed_screened','prohib_failed_screened','prohib','unavailable'), 
    callerid_tag VARCHAR(40), 
    `100rel` ENUM('no','required','yes'), 
    aggregate_mwi ENUM('yes','no'), 
    trust_id_inbound ENUM('yes','no'), 
    trust_id_outbound ENUM('yes','no'), 
    use_ptime ENUM('yes','no'), 
    use_avpf ENUM('yes','no'), 
    media_encryption ENUM('no','sdes','dtls'), 
    inband_progress ENUM('yes','no'), 
    call_group VARCHAR(40), 
    pickup_group VARCHAR(40), 
    named_call_group VARCHAR(40), 
    named_pickup_group VARCHAR(40), 
    device_state_busy_at INTEGER, 
    fax_detect ENUM('yes','no'), 
    t38_udptl ENUM('yes','no'), 
    t38_udptl_ec ENUM('none','fec','redundancy'), 
    t38_udptl_maxdatagram INTEGER, 
    t38_udptl_nat ENUM('yes','no'), 
    t38_udptl_ipv6 ENUM('yes','no'), 
    tone_zone VARCHAR(40), 
    language VARCHAR(40), 
    one_touch_recording ENUM('yes','no'), 
    record_on_feature VARCHAR(40), 
    record_off_feature VARCHAR(40), 
    rtp_engine VARCHAR(40), 
    allow_transfer ENUM('yes','no'), 
    allow_subscribe ENUM('yes','no'), 
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
    dtls_setup ENUM('active','passive','actpass'), 
    srtp_tag_32 ENUM('yes','no'), 
    UNIQUE (id)
);

CREATE INDEX ps_endpoints_id ON ps_endpoints (id);

CREATE TABLE ps_auths (
    id VARCHAR(40) NOT NULL, 
    auth_type ENUM('md5','userpass'), 
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
    remove_existing ENUM('yes','no'), 
    qualify_frequency INTEGER, 
    authenticate_qualify ENUM('yes','no'), 
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
    `match` VARCHAR(80), 
    UNIQUE (id)
);

CREATE INDEX ps_endpoint_id_ips_id ON ps_endpoint_id_ips (id);

UPDATE alembic_version SET version_num='43956d550a44';

-- Running upgrade 43956d550a44 -> 581a4264e537

CREATE TABLE extensions (
    id BIGINT NOT NULL, 
    context VARCHAR(40) NOT NULL, 
    exten VARCHAR(40) NOT NULL, 
    priority INTEGER NOT NULL AUTO_INCREMENT, 
    app VARCHAR(40) NOT NULL, 
    appdata VARCHAR(256) NOT NULL, 
    PRIMARY KEY (context, exten, priority), 
    UNIQUE (id)
);

UPDATE alembic_version SET version_num='581a4264e537';


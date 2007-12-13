grammar http;
options
{ 
	k = 2; 
}

// Tokens

SP 					:	' ' ;
HT					:	'\t' ;
CR					: 	'\r' ;
LF					: 	'\n' ;
CRLF 				:	CR LF;
DIGIT 				:	'0'..'9' ;	
ALPHANUM			: 	ALPHA | DIGIT ;
ALPHA				: 	LOWALPHA | UPALPHA ;
fragment LOWALPHA	:	'a'..'z' ;
fragment UPALPHA	:	'A'..'Z' ; 
CTL					:	'\u0000'..'\u0037' | '\u0177' ;
OCTET			    :	'\u0000'..'\u00FF' ;
CHAR				:	'\u0000'..'\u0177' ;
LWSP_CHAR			:	SP | HT ;
LWSP	   			:  	(CRLF)? LWSP_CHAR+ ;
HEX					: 	DIGIT | 'A' | 'B' | 'C' | 'D' | 'E' | 'F' | 'a' | 'b' | 'c' | 'd' | 'e' | 'f' ;                           
SEPARATORS     		:	'(' | ')' | '<' | '>' | '@' | ',' | ';' | ':' | '\\' | '"' | '/' | '[' | ']' | '?' | '=' | '{' | '}' | SP | HT  ;
TOKEN   			: 	~(SEPARATORS | CTL);
QDTEXT         		: 	~('"' | CTL) ;
CTEXT				:	~('(' | ')') ;
SPECIALS    			:  	'(' | ')' | '<' | '>' | '@' |  ',' | ';' | ':' | '\\' | '"' | '.' | '[' | ']' ;
DTEXT					:	~( '[' | ']' | '\\' | CR ) ;

quoted_string  		:	'"' (QDTEXT | quoted_pair )* '"' ;
quoted_pair			:	'\\' TOKEN ;

comment				:	'(' ( CTEXT | quoted_pair | comment )* ')' ;

qvalue				:	( '0' ( '.' (d+=DIGIT)* {$d.size() <=3 }? )? ) | ( '1' ( '.' ('0' | '00' | '000' )? )? ) ;


// URI

RESERVED			: 	';' | '/' | '?' | ':' | '@' | '&' | '=' | '+' | '$' | ',' ;
UNRESERVED			: 	ALPHANUM | MARK ;
MARK				: 	'-' | '_' | '.' | '!' | '~' | '*' | '\'' | '(' | ')' ;

uri_reference 		: 	( absoluteURI | relativeURI )? ( '#' frag )? ;

absoluteURI			: 	scheme ':' ( hier_part | opaque_part ) ;
relativeURI			: 	( net_path | abs_path | rel_path ) ( '?' query )? ;

hier_part			: 	( net_path | abs_path ) ( '?' query )? ;
opaque_part			: 	uric_no_slash uric* ;

uric_no_slash			: 	UNRESERVED | escaped | ';' | '?' | '=' | '@' | '&' | ':' | '+' | '$' | ',' ;

net_path			: 	'//' authority ( abs_path )? ;
abs_path			: 	'/'  path_segments ;
rel_path			: 	rel_segment ( abs_path )? ;

rel_segment			: 	( UNRESERVED | escaped | ';' | '@' | '&' | '=' | '+' | '$' | ',' )+ ;

scheme				: 	ALPHA ( ALPHA | DIGIT | '+' | '-' | '.' )* ;

//authority			: 	server | reg_name ;
authority			: 	server ;

reg_name			: 	( UNRESERVED | escaped | '$' | ',' | ';' | ':' | '@' | '&' | '=' | '+' )+ ;

server				: 	( ( userinfo '@' )? hostport )? ;
userinfo			: 	( UNRESERVED | escaped | ';' | ':' | '&' | '=' | '+' | '$' | ',' )* ;

hostport			: 	uri_host ( ':' port )? ;
uri_host			: 	hostname | ipv4address ;
hostname			: 	( domainlabel '.' )* toplabel ( '.' )? ;
domainlabel			: 	ALPHANUM | ALPHANUM ( ALPHANUM | '-' )* ALPHANUM ;
toplabel			: 	ALPHA | ALPHA ( ALPHANUM | '-' )* ALPHANUM ;
ipv4address			: 	DIGIT+ '.' DIGIT+ '.' DIGIT+ '.' DIGIT+ ;
port				: 	DIGIT* ;

path				: 	( abs_path | opaque_part )? ;
path_segments			: 	segment ( '/' segment )* ;
segment				: 	pchar* ( ';' param )* ;
param				: 	pchar* ;
pchar				: 	UNRESERVED | escaped | ':' | '@' | '&' | '=' | '+' | '$' | ',' ;

query				: 	uric* ;

uric				: 	RESERVED | UNRESERVED | escaped ;

escaped				: 	'%' HEX HEX ;

frag      			: 	uric* ;

// Auth
credentials			:	auth_scheme ( auth_param ( ',' auth_param )* )? ;
auth_scheme			:	TOKEN ;
auth_param			:	TOKEN '=' ( TOKEN | quoted_string ) ;
challenge			:	auth_scheme SP+ ( auth_param ( ',' auth_param )* )? ;
realm				:	'realm' '=' realm_value ;
realm_value			:	quoted_string ;

// Address

mailbox					:  	addr_spec | (phrase)? route_addr ;
fragment atom        			:	~(SPECIALS | SP | CTL)+ ;
phrase					:	word+ ;
word					:	atom | quoted_string ;
route_addr				:	'<' ( route )? addr_spec '>' ;
route					:	'@' domain (',' '@' domain)* ':' ;
addr_spec				:	local_part '@' domain ;
local_part				:	word ('.' word)* ;
domain					:	sub_domain ('.' sub_domain)* ;
sub_domain				:	domain_ref | domain_literal ;
domain_ref				:	atom ; 
domain_literal 			:	'[' (DTEXT | quoted_pair)* ']' ;

// Request

request					:	request_line (( general_header | request_header | entity_header ) CRLF )* CRLF ( message_body )? ;

request_line 			:	method SP request_uri SP http_version CRLF;
						
method 					:	'OPTIONS'
			                | 'GET'
			                | 'HEAD'                   
			                | 'POST'
			                | 'PUT'
					        | 'DELETE'
			                | 'TRACE'
			                | 'CONNECT'
			                ;
		
request_uri				:	'*' 
							| absoluteURI 
							| abs_path 
							| authority
							;
							
			
		
http_version 		:	'HTTP' '/' DIGIT+ '.' DIGIT+ ;
						
general_header 		:       
						cahce_control
                      	| connection
                      	| date
                      	| pragma
                      	| trailer
                      	| transfer_encoding
                      	| upgrade
                      	| via
                      	| warning
                      	;
                      	
 request_header		:		
						accept
                      	| accept_charset
                      	| accept_encoding
                      	| accept_language
                      	| authorization
                      	| expect
                      	| from
                      	| host
                      	| if_match
                      	| if_modified_since
                      	| if_none_match
                      	| if_range
                      	| if_unmodified_since
                      	| max_forwards
                      	| proxy_authorization
                      	| range
                      	| referer
                      	| te
                      	| user_agent
                      	;
                      	
entity_header 			:	allow 
							| content_encoding 
							| content_language 
							| content_length 
							| content_location 
							| content_md5 
							| content_range
							| content_type
							| expires
							| last_modified
							| extension_header
							;

// Header definitions 
message_header			:	field_name ':' ( field_value )? ;
field_name				:	TOKEN ;
field_value				:	( field_content | LWSP ) * ;
field_content			:	(ALPHANUM | SEPARATORS | LWSP | quoted_string )* ;
                        
cahce_control			:	'Cache-Control' ':' cache_directive ( ',' cache_directive )*;
cache_directive 		:	cache_request_directive | cache_response_directive ;
cache_request_directive :	
		           			'no-cache' 
		           			| 'no-store'
		           			| 'max-age' '=' delta_seconds 
		           			| 'max-stale' ( '=' delta_seconds )?   
		           			| 'min-fresh' '=' delta_seconds
		         			| 'no-transform'
		         			| 'only-if-cached'	
		         			| cache_extension                     
		         			;
		 
delta_seconds  			:	DIGIT+ ;
cache_response_directive :
							'public' 
							| 'private' ( '=' '"' field_name '"' ( ',' '"' field_name '"' )*  )? 
							| 'no-cache' ( '=' '"' field_name '"' ( ',' '"' field_name '"' )*  )?
							| 'no-store'
							| 'no-transform'
							| 'must-revalidate'
							| 'proxy-revalidate'
							| 'max-age' '=' delta_seconds 
							| 's-maxage' '=' delta_seconds           
							| cache_extension                        
							;	
cache_extension 		:	 TOKEN ( '=' ( TOKEN | quoted_string ) )? ;
		
		
connection 				:	'Connection' ':' connection_token ( ',' connection_token )* ;
connection_token  		: 	TOKEN ;

date  					:	'Date' ':' http_date ;
http_date    			:	rfc1123_date | rfc850_date | asctime_date ;
rfc1123_date 			:	wkday ',' SP date1 SP time SP 'GMT' ;
rfc850_date  			: 	weekday ',' SP date2 SP time SP 'GMT' ;
asctime_date 			: 	wkday SP date3 SP time SP DIGIT DIGIT DIGIT DIGIT ;
date1        			: 	DIGIT DIGIT SP month SP DIGIT DIGIT DIGIT DIGIT ;
date2        			: 	DIGIT DIGIT '_' month '_' DIGIT DIGIT ;
date3        			:	month SP ( DIGIT DIGIT | ( SP DIGIT )) ;
time         			:	DIGIT DIGIT ':' DIGIT DIGIT ':' DIGIT DIGIT  ;
wkday        			: 	'Mon' | 'Tue' | 'Wed' | 'Thu' | 'Fri' | 'Sat' | 'Sun' ;
weekday      			: 	'Monday' | 'Tuesday' | 'Wednesday' | 'Thursday' | 'Friday' | 'Saturday' | 'Sunday' ;
month        			: 	'Jan' | 'Feb' | 'Mar' | 'Apr' | 'May' | 'Jun' | 'Jul' | 'Aug' | 'Sep' | 'Oct' | 'Nov' | 'Dec' ;

pragma					:	'Pragma' ':' pragma_directive (',' pragma_directive)* ;
pragma_directive		:	'no_cache' | extension_pragma ;
extension_pragma		:	TOKEN ( '=' ( TOKEN | quoted_string ) )? ;

trailer					:	'Trailer' ':' field_name ( ',' field_name )* ;

transfer_encoding       :	'Transfer-Encoding' ':' transfer_coding ( ',' transfer_coding )* ;
transfer_coding			:	'chunked' | transfer_extension ;
transfer_extension      :	TOKEN ( ';' parameter )* ;
parameter               :	attribute '=' value ;
attribute               :	TOKEN ;
value                   :	TOKEN | quoted_string ;

upgrade					:	'Upgrade' ':' product ( ',' product )* ;
product					:	TOKEN ('/' product_version)? ;
product_version			:	TOKEN ;

via						:	'Via' ':' received_protocol received_by ( comment )? ( ',' received_protocol received_by ( comment )? ) ;
received_protocol		:	(protocol_name '/' )? protocol_version ;
protocol_name			:	TOKEN ;
protocol_version		:	TOKEN ;
received_by				:	( host ( ':' port )? ) | pseudonym ;
pseudonym				:	TOKEN ;

warning					:	'Warning' ':' warning_value ( ',' warning_value )* ;
warning_value			:	warn_code SP warn_agent SP warn_text [SP warn-date] ;
warn_code				:	DIGIT DIGIT DIGIT ;
warn_agent				:	( host ( ':' port )? ) | pseudonym ;
warn_text				:	quoted_string ;
warn_date				:	'"' http_date '"' ;

accept					:	'Accept' ':' ( media_range ( accept_params )?  ( ',' media_range ( accept_params )? )* )? ;
media_range				:	(options { backtrack = true; } : ( '*/*' | ( type '/' '*' ) | ( type '/' subtype )) ( ';' parameter )* ) ;
accept_params			:	';' 'q' '=' qvalue ( accept_extension )* ;
accept_extension		:	';' TOKEN ( '=' ( TOKEN | quoted_string ) )? ;
media_type				:	type '/' subtype ( ';' parameter )* ;
type					:	TOKEN ;
subtype					:	TOKEN ;

accept_charset			:	'Accept_Charset' ':' ( charset | '*' )( ';' 'q' '=' qvalue )? ( ( charset | '*' )( ';' 'q' '=' qvalue )? )* ;
charset					:	TOKEN ;
              
accept_encoding			:	'Accept_Encoding' ':' codings ( ';' 'q' '=' qvalue )? ( ',' codings ( ';' 'q' '=' qvalue )? )* ;
codings					:	content_coding | '*' ;
content_coding			:	TOKEN ;

accept_language			:	'Accept_Language' ':' language_range ( ';' 'q' '=' qvalue )? ( ',' language_range ( ';' 'q' '=' qvalue )? )* ;
language_range			:	( (a+=ALPHA)+ {$a.size() <= 8}?  ( '-' (a+=ALPHA)+ {$a.size() <= 8}? )* ) | '*' ;

authorization			:	'Authorization' ':' credentials ;

expect					:	'Expect' ':' expectation ( ',' expectation)* ;
expectation				:	'100-continue' | expectation_extension ;
expectation_extension	:	TOKEN ( '=' ( TOKEN | quoted_string ) expect_params* )? ;
expect_params			:	';' TOKEN ( '=' ( TOKEN | quoted_string ) )? ;

from					:	'From' ':' mailbox ;

host 					:	'Host' ':' host ( ':' port ) ;

if_match 				:	'If_Match' ':' ( '*' | entity_tag ( ',' entity_tag )* ) ;
entity_tag 				: 	weak? opaque_tag ;
weak       				: 	'W/' ;
opaque_tag 				:	quoted_string ;

if_modified_since 		: 	'If-Modified-Since' ':' http_date ;

if_none_match 			: 	'If-None-Match' ':' ( '*' | entity_tag ( ',' entity_tag )* ) ;

if_range 				: 	'If-Range' ':' ( entity_tag | http_date ) ;

if_unmodified_since 	: 	'If-Unmodified-Since' ':' http_date ;

max_forwards   			: 	'Max-Forwards' ':' DIGIT+ ;

proxy_authorization     :	'Proxy-Authorization' ':' credentials ;

range 					: 	'Range' ':' ranges_specifier ;
ranges_specifier		:	byte_ranges_specifier ;
byte_ranges_specifier 	:	bytes_unit '=' byte_range_set ;
byte_range_set			:	( byte_range_spec | suffix_byte_range_spec ) ( ',' ( byte_range_spec | suffix_byte_range_spec ) )* ;
byte_range_spec			:	first_byte_pos '-' (last_byte_pos)? ;
first_byte_pos			:	DIGIT+ ;
last_byte_pos			:	DIGIT+ ;
suffix_byte_range_spec	:	'-' suffix_length ;
suffix_length			:	DIGIT+ ;
range_unit				:	bytes_unit | other_range_unit ;
bytes_unit				:	'bytes' ;
other_range_unit		:	TOKEN ;

referer					:	'Referer' ':' ( absoluteURI | relativeURI ) ;

te        				: 	'TE' ':' ( t_codings (',' t_codings)* )? ;
t_codings 				:	'trailers' | ( transfer_extension ( accept_params )? ) ;

user_agent				:	'User_Agent' ':' ( product | comment )+ ;

allow					:	'Allow' ':' ( method ( ',' method )* )? ;

content_encoding		:	'Content-Encoding' ':' content_coding ( ',' content_coding )* ;

content_language		:	'Content-Language' ':' language_tag ( ',' language_tag )* ;
language_tag			:	primary_tag ( '-' subtag )* ;
primary_tag				:	(a+=ALPHA)+ {$a.size() <= 8}? ;
subtag					:	(a+=ALPHA)+ {$a.size() <= 8}? ;

content_length			:	'Content-Length' ':' DIGIT+ ;

content_location		:	'Content-Location' ':' ( absoluteURI | relativeURI ) ;

content_md5				:	'Content-MD5' ':' md5_digest ;
md5_digest				:	( ALPHANUM | '=' )+ ;

content_range			:	'Content-Range' ':' content_range_spec ;
content_range_spec      :	byte_content_range_spec ;
byte_content_range_spec :	bytes_unit SP byte_range_resp_spec '/' ( instance_length | '*' ) ;
byte_range_resp_spec	:	(first_byte_pos '-' last_byte_pos) | '*' ;
instance_length         :	DIGIT+ ;

content_type			:	'Content-Type' ':' media_type ;

expires					:	'Expires' ':' http_date ;

last_modified			:	'Last-Modified' ':' http_date ;

extension_header		:	message_header ;

// Message Body

message_body			:	entity_body | encoded_entity_body ;
entity_body				:	OCTET* ;
encoded_entity_body		:	chunked_body ;

chunked_body			:	chunk* last_chunk chunked_trailer CRLF ;
chunk					:	chunk_size ( chunk_extension )? CRLF chunk_data CRLF ;
chunk_size				:	HEX+ ;
last_chunk				:	('0')+ ( chunk_extension )? CRLF ;
chunk_extension			:	( ';' chunk_ext_name ( '=' chunk_ext_val )? )* ;
chunk_ext_name			:	TOKEN ;
chunk_ext_val			:	TOKEN | quoted_string ;
chunk_data				:	OCTET* ;
chunked_trailer			:	(entity_header CRLF)* ;


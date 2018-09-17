#ifndef __GUARD_CHECKCOPIEDCONSTANTS_H_
#define __GUARD_CHECKCOPIEDCONSTANTS_H_

#include <weechat-plugin.h>
#include "java/eu_rationality_thetruth_Weechat.h"

#if eu_rationality_thetruth_Weechat_WEECHAT_RC_OK != WEECHAT_RC_OK
	#error "Java constant and native constant do not match for: WEECHAT_RC_OK"
#endif

int fooooo = eu_rationality_thetruth_Weechat_WEECHAT_RC_OK;
int baaaar = WEECHAT_RC_OK;

#if eu_rationality_thetruth_Weechat_WEECHAT_RC_OK_EAT != WEECHAT_RC_OK_EAT
	#error "Java constant and native constant do not match for: WEECHAT_RC_OK_EAT"
#endif


#if eu_rationality_thetruth_Weechat_WEECHAT_RC_ERROR != WEECHAT_RC_ERROR
	#error "Java constant and native constant do not match for: WEECHAT_RC_ERROR"
#endif


#if eu_rationality_thetruth_Weechat_WEECHAT_CONFIG_READ_OK != WEECHAT_CONFIG_READ_OK
	#error "Java constant and native constant do not match for: WEECHAT_CONFIG_READ_OK"
#endif


#if eu_rationality_thetruth_Weechat_WEECHAT_CONFIG_READ_MEMORY_ERROR != WEECHAT_CONFIG_READ_MEMORY_ERROR
	#error "Java constant and native constant do not match for: WEECHAT_CONFIG_READ_MEMORY_ERROR"
#endif


#if eu_rationality_thetruth_Weechat_WEECHAT_CONFIG_READ_FILE_NOT_FOUND != WEECHAT_CONFIG_READ_FILE_NOT_FOUND
	#error "Java constant and native constant do not match for: WEECHAT_CONFIG_READ_FILE_NOT_FOUND"
#endif


#if eu_rationality_thetruth_Weechat_WEECHAT_CONFIG_WRITE_OK != WEECHAT_CONFIG_WRITE_OK
	#error "Java constant and native constant do not match for: WEECHAT_CONFIG_WRITE_OK"
#endif


#if eu_rationality_thetruth_Weechat_WEECHAT_CONFIG_WRITE_ERROR != WEECHAT_CONFIG_WRITE_ERROR
	#error "Java constant and native constant do not match for: WEECHAT_CONFIG_WRITE_ERROR"
#endif


#if eu_rationality_thetruth_Weechat_WEECHAT_CONFIG_WRITE_MEMORY_ERROR != WEECHAT_CONFIG_WRITE_MEMORY_ERROR
	#error "Java constant and native constant do not match for: WEECHAT_CONFIG_WRITE_MEMORY_ERROR"
#endif


#if eu_rationality_thetruth_Weechat_WEECHAT_CONFIG_OPTION_SET_OK_CHANGED != WEECHAT_CONFIG_OPTION_SET_OK_CHANGED
	#error "Java constant and native constant do not match for: WEECHAT_CONFIG_OPTION_SET_OK_CHANGED"
#endif


#if eu_rationality_thetruth_Weechat_WEECHAT_CONFIG_OPTION_SET_OK_SAME_VALUE != WEECHAT_CONFIG_OPTION_SET_OK_SAME_VALUE
	#error "Java constant and native constant do not match for: WEECHAT_CONFIG_OPTION_SET_OK_SAME_VALUE"
#endif


#if eu_rationality_thetruth_Weechat_WEECHAT_CONFIG_OPTION_SET_ERROR != WEECHAT_CONFIG_OPTION_SET_ERROR
	#error "Java constant and native constant do not match for: WEECHAT_CONFIG_OPTION_SET_ERROR"
#endif


#if eu_rationality_thetruth_Weechat_WEECHAT_CONFIG_OPTION_SET_OPTION_NOT_FOUND != WEECHAT_CONFIG_OPTION_SET_OPTION_NOT_FOUND
	#error "Java constant and native constant do not match for: WEECHAT_CONFIG_OPTION_SET_OPTION_NOT_FOUND"
#endif


#if eu_rationality_thetruth_Weechat_WEECHAT_CONFIG_OPTION_UNSET_OK_NO_RESET != WEECHAT_CONFIG_OPTION_UNSET_OK_NO_RESET
	#error "Java constant and native constant do not match for: WEECHAT_CONFIG_OPTION_UNSET_OK_NO_RESET"
#endif


#if eu_rationality_thetruth_Weechat_WEECHAT_CONFIG_OPTION_UNSET_OK_RESET != WEECHAT_CONFIG_OPTION_UNSET_OK_RESET
	#error "Java constant and native constant do not match for: WEECHAT_CONFIG_OPTION_UNSET_OK_RESET"
#endif


#if eu_rationality_thetruth_Weechat_WEECHAT_CONFIG_OPTION_UNSET_OK_REMOVED != WEECHAT_CONFIG_OPTION_UNSET_OK_REMOVED
	#error "Java constant and native constant do not match for: WEECHAT_CONFIG_OPTION_UNSET_OK_REMOVED"
#endif

#if eu_rationality_thetruth_Weechat_WEECHAT_CONFIG_OPTION_UNSET_ERROR != WEECHAT_CONFIG_OPTION_UNSET_ERROR
	#error "Java constant and native constant do not match for: WEECHAT_CONFIG_OPTION_UNSET_ERROR"
#endif

#endif

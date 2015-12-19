#ifndef EQDALAYA_FUNCTIONS_HPP
#define EQDALAYA_FUNCTIONS_HPP

#ifdef __cplusplus
extern "C" {
#endif

// CHotButtonWnd::DoHotButton((short,int))
#define EVERQUEST_FUNCTION_CHotButtonWnd__DoHotButton 0x004F11F9

#ifdef EVERQUEST_FUNCTION_CHotButtonWnd__DoHotButton
typedef void (__stdcall *_everquest_function_CHotButtonWnd__DoHotButton)(short button_index, int allow_auto_right_click);
_everquest_function_CHotButtonWnd__DoHotButton everquest_function_CHotButtonWnd__DoHotButton = (_everquest_function_CHotButtonWnd__DoHotButton)EVERQUEST_FUNCTION_CHotButtonWnd__DoHotButton;
#endif

// CEverQuest::dsp_chat((char const *))
#define EVERQUEST_FUNCTION_CEverQuest__dsp_chat 0x00497105

#ifdef EVERQUEST_FUNCTION_CEverQuest__dsp_chat
typedef void (__stdcall *_everquest_function_CEverQuest__dsp_chat)(char* text);
static _everquest_function_CEverQuest__dsp_chat everquest_function_CEverQuest__dsp_chat = (_everquest_function_CEverQuest__dsp_chat)EVERQUEST_FUNCTION_CEverQuest__dsp_chat;
#endif

#ifdef __cplusplus
}
#endif

#endif // EQDALAYA_FUNCTIONS_HPP

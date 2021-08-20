#include "framework.h"
#include "ParsecKorHelper.h"

using namespace std;

HICON AppIcon = NULL;

const wchar_t* APP_TITLE = L"Parsec 한영 도우미";

constexpr int HOTKEY_ID_HANGUL = 1;
constexpr int HOTKEY_ID_HANJA = 2;

// 한글 키 조합
constexpr UINT HOTKEY_HANGUL_MOD = MOD_CONTROL | MOD_ALT | MOD_NOREPEAT;
constexpr UINT HOTKEY_HANGUL_VK = VK_SPACE;
// 한자 키 조합: 한글 입력 도중 ALT, WIN키 입력이 들어오면 IME가 조합을 완료시켜서 사용하지 못함
constexpr UINT HOTKEY_HANJA_MOD = MOD_CONTROL | MOD_SHIFT;
constexpr UINT HOTKEY_HANJA_VK = VK_RETURN;

INT_PTR CALLBACK AppDlgProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	UNREFERENCED_PARAMETER(lparam);

	switch (msg) {
	case WM_INITDIALOG:
	{
		SendMessage(hwnd, WM_SETICON, ICON_SMALL, LPARAM(AppIcon));
		SetWindowText(hwnd, APP_TITLE);

		// 핫키 설정
		auto regHotkey = [&](int id, UINT mod, UINT vk, const wchar_t* desc) {
			if (!RegisterHotKey(hwnd, id, mod, vk)) {
				wstringstream ss;
				ss << desc << L" 단축키 등록 실패: ";
				auto err = GetLastError();
				if (err == 1409)
					ss << desc << L" 이미 사용 중인 키조합";
				else
					ss << desc << L" Error: " << err;
				return false;
			}
			return true;
		};

		if (!regHotkey(HOTKEY_ID_HANGUL, HOTKEY_HANGUL_MOD, HOTKEY_HANGUL_VK, L"한글") || !regHotkey(HOTKEY_ID_HANJA, HOTKEY_HANJA_MOD, HOTKEY_HANJA_VK, L"한자")) {
			EndDialog(hwnd, 1);
			return FALSE;
		}

		return TRUE;
	}

	case WM_HOTKEY:
		switch (wparam) {
		case HOTKEY_ID_HANGUL:
			keybd_event(VK_HANGUL, 0xF2, KEYEVENTF_KEYUP, 0);
			return 0;

		case HOTKEY_ID_HANJA:
			keybd_event(VK_HANJA, 0xF1, KEYEVENTF_KEYUP, 0);
			return 0;
		}
		break;

	case WM_CLOSE:
		EndDialog(hwnd, 0);
		return 0;

	case WM_COMMAND:
		if (wparam == IDOK)
			EndDialog(hwnd, 0);
		return 0;

	case WM_DESTROY:
		// 핫키 해제
		UnregisterHotKey(hwnd, HOTKEY_ID_HANGUL);
		UnregisterHotKey(hwnd, HOTKEY_ID_HANJA);
		return 0;
	}

	return FALSE;
}

int WINAPI wWinMain(_In_ HINSTANCE hinst, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int) {
	AppIcon = LoadIcon(hinst, MAKEINTRESOURCE(IDI_APP));

	return int(DialogBoxParam(hinst, MAKEINTRESOURCE(IDD_APP), NULL, AppDlgProc, 0));
}
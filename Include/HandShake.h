// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� HANDSHAKE_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// HANDSHAKE_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef HANDSHAKE_EXPORTS
#define HANDSHAKE_API __declspec(dllexport)
#else
#define HANDSHAKE_API __declspec(dllimport)
#endif

HANDSHAKE_API int fnGetCheckData(unsigned char CheckData[16]);
HANDSHAKE_API bool fnHandShakeConfirm(unsigned char ConfirmData[16]);
#ifndef __XH_LDS_LM_H_
#define __XH_LDS_LM_H_
// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
//��ı�׼�������� DLL �е������ļ��������������϶���� XHLDSLM_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
//�κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ 
// XHLDSLM_API ������Ϊ�ǴӴ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef APP_EMBEDDED_MODULE_ENCRYPT
#define XHLDSLM_API	//��ǶʱXHLDSLM_APIʲô��������
#else
#ifdef XHLDSLM_EXPORTS
#define XHLDSLM_API __declspec(dllexport)
#else
#define XHLDSLM_API __declspec(dllimport)
#endif
#endif
#include "..\XhLicAgent\XhLicAgent.h"

struct CFGLEG;
class XHLDSLM_API CFGWORD{
public:
	union{
		DWORD word[6];
		BYTE bytes[24];
	}flag;
	CFGWORD(){memset(&flag,0,24);}
	CFGWORD(int iNo){SetWordByNo(iNo);}
	CFGWORD(int iBodyNo,int iLegNo,BYTE schema=0);
	CFGWORD ConvertSchema(BYTE schemaFrom,BYTE schemaTo,char* sError=NULL,UINT uStrBufLen=0) const;
	CFGLEG ToCfgLeg(BYTE schema=0);
	void Clear(){memset(&flag,0,24);}
	BOOL And(CFGWORD wcfg) const;	//�൱���á�&�����ж��Ƿ��н���
	CFGWORD operator &(CFGWORD wcfg) const;
	CFGWORD operator &=(CFGWORD wcfg);
	CFGWORD ToBodyWord(BYTE schema=0) const;	//����Ϊ���������ĺţ���������Ч���ߵ����н���λ��
	BOOL IsHasNo(int iNo);			//��������Ƿ���ָ����iNo��λ
	BOOL IsHasBodyNoOnly(int iBodyNo,BYTE schema=0);	//��������Ƿ��ָ���ֽ�����ֵ������Ϊ0��iByte��1Ϊ����(������ֵ+1)
	BOOL IsHasBodyNo(int iBodyNo,BYTE schema=0);		//�������ָ���ֽ��Ƿ���ֵ������Ϊ0��iByte��1Ϊ����(������ֵ+1)
	BOOL AddBodyLegs(int iBodyNo,DWORD legword=0xffffff,BYTE schema=0);
	BOOL SetBodyLegs(int iBodyNo,DWORD legword=0xffffff,BYTE schema=0);
	DWORD GetBodyLegs(int iBodyNo,BYTE schema=0);
	BOOL IsEqual(CFGWORD cfgword,bool bVerifyBodyNo=false,BYTE schema=0);	//bVerifyBodyNo=true,ʱֻ��֤��Ӧ�ֽ��Ƿ���ֵ����ֵ
	BOOL IsNull(){return flag.word[0]==0&&flag.word[1]==0&&flag.word[2]==0&&flag.word[3]==0&&flag.word[4]==0&&flag.word[5]==0;}
	void SetWord(CFGWORD cfgword);				//ֱ��ָ�������
	CFGWORD SetWordByNo(int iNo);			//����ָ����iNo��λָ�������
	CFGWORD AddSpecNoWord(int iNo);			//������������ָ���ĺ�λ
	CFGWORD RemoveSpecNoWord(int iNo);		//����������Ƴ�ָ���ĺ�λ
	CFGWORD AddSpecWord(CFGWORD cfgword);	//�൱����������ֽ��л����,���������ֵ����ǰ�����
	CFGWORD RemoveSpecWord(CFGWORD cfgword);//�ӵ�ǰ��������Ƴ���ָ������ֹ��еĽ�����λ
	int GetExistBodyNos(BYTE* parrBodyNos,int maxcount=0);
	int GetExistLegSerials(BYTE* parrLegSerials,int maxcount=0);
	int GetBodyScopeStr(char* scopeStr,int maxCount=51,BYTE schema=0);
	int GetLegScopeStr(char* scopeStr,int maxCount=17,bool bIncBodyNo=false,BYTE schema=0);
};
struct XHLDSLM_API CFGLEG{
private:
	union{
		DWORD flag;
		BYTE bytes[4];
	}code;
	static BYTE MULTILEG_SCHEMA;	//��ǰĬ�ϵĺ��߽���ռλ����ģʽ
public:	//����߶���ȵ�ռλ����ģʽ
	static const BYTE MULTILEG_DEFAULT	= 0;
	static const BYTE MULTILEG_MAX08	= 1;
	static const BYTE MULTILEG_MAX16	= 2;
	static const BYTE MULTILEG_MAX24	= 3;
	static BYTE Schema(){return MULTILEG_SCHEMA;}
	static BYTE SetSchema(BYTE cbMultiLegSchema);
	static BYTE ValidateSchema(BYTE schema);	//ȷ������ģʽΪ�Ϸ����߽���ռλ����ģʽ
	static BYTE MaxLegs(BYTE schema=MULTILEG_DEFAULT);	//ָ��ģʽ֧�������߽�����
	static BYTE MaxBodys(BYTE schema=MULTILEG_DEFAULT);	//ָ��ģʽ֧�������߱�����
public:
	CFGLEG();	//Ĭ��1A����
	CFGLEG(DWORD flag);	//Ĭ��1A����
	CFGLEG(BYTE iBodyNo,BYTE iLegNo,BYTE schema=MULTILEG_DEFAULT);
	CFGLEG ConvertSchema(BYTE schemaFrom,BYTE schemaTo,char* sError=NULL,UINT uStrBufLen=0) const;
	CFGWORD ToCfgWord(BYTE schema=MULTILEG_DEFAULT);
	operator DWORD(){return code.flag;}
	DWORD BodyFlag(BYTE schema=MULTILEG_DEFAULT) const;
	DWORD LegFlag (BYTE schema=MULTILEG_DEFAULT) const;
	bool IsHasNo(int iBodyNo,int iLegNo=1,bool bVerifyLegNo=false,BYTE schema=MULTILEG_DEFAULT);
	CFGLEG AddBodyNo(int iBodyNo,BYTE schema=MULTILEG_DEFAULT);
	CFGLEG AddLegNo(int iLegNo,BYTE schema=MULTILEG_DEFAULT);
	CFGLEG AddSpecWord(CFGLEG bodyleg);	//�൱����������ֽ��л����,���������ֵ����ǰ�����
	CFGLEG RemoveSpecNoWord(int iNo,bool bLegNo,BYTE schema=MULTILEG_DEFAULT);
	CFGLEG SetBodyFlag(DWORD bodyFlag,BYTE schema=MULTILEG_DEFAULT);
	CFGLEG SetLegFlag(DWORD legFlag,BYTE schema=MULTILEG_DEFAULT);
	int GetBodyScopeStr(char* scopeStr,int maxCount=51,BYTE schema=MULTILEG_DEFAULT);
	int GetLegScopeStr(char* scopeStr,int maxCount=17,BYTE schema=MULTILEG_DEFAULT);
};

XHLDSLM_API int ImportLicFile(char* licfile,BYTE cProductType,DWORD version[2]);
//�������ʽ��ΪCFGWORD������ĺŶ���
XHLDSLM_API CFGWORD GetCfgWord(long iNo);	//��1��ʼ����
XHLDSLM_API int MakeByteToCharStr(BYTE ch,char* cfg_str,char baseChar='A');
XHLDSLM_API int MakeBytesToString(BYTE* byteArr,int bytes_size,char *cfg_str,int max_buf_len);//������ֽ�λת��Ϊ�ַ�����ʽ����ĺ�
XHLDSLM_API int MakeWordToString(DWORD dwPartNo,char *cfg_str); //��DWORD ת��Ϊ�ַ�����ʽ����ĺ�
XHLDSLM_API int MakeWordToString(CFGWORD dwCfgPartNo,char *cfg_str); //��CFGWORD ת��Ϊ�ַ�����ʽ����ĺ�
XHLDSLM_API bool SimplifiedNumString(char *num_str);
//radius:Բ���뾶��sector_angle:Բ�����νǣ�scale_of_user2scr:ʵ������Ļ��ʾ�ĳߴ������sample_len��Ļ��ʾԲ���ĳ������ȣ�smoothnessԲ���������ʾƬ��
XHLDSLM_API int CalArcResolution(double radius,double sector_angle,double scale_of_user2scr,double sample_len,int smoothness=36);
//���뾫ȷ��񣬷��ع�������ӽ��ı�׼���ְ��
XHLDSLM_API int FindPlankThick(double f_thick);
XHLDSLM_API BOOL IsMatch(char ch, char charSet[]);//�ж�cahrSet���Ƿ�����ch��ƥ����ַ�
XHLDSLM_API double CalSteelWeight(double vol);

#include "f_ent.h"

class XHLDSLM_API MIRMSG{
public:
	BYTE axis_flag;		//0x01:X��;0x02:Y��;0x04:Z��;0x08:��ת�Գ�;0x10:ƽ�澵��
	BYTE array_num;		//��ת�ԳƵĴ���
	short rotate_angle;	//����ת��(Z��)��ת�Ƕ�(��Ƕ���)
	GEPOINT origin,mir_norm;	//�Գƻ��㣬��ת�᷽�����Գ�ƽ�淨��
public:
	MIRMSG();
	MIRMSG(int mir_x1_y2_z3);
	MIRMSG GetSubMirItem(int mir_x1_y2_z3_p4);	//�������4ʱ��ʾ����Գ�(���ƽ��mir_norm)
	short ValidRotAngle(){return (axis_flag&0x08)==0 ? 0 : rotate_angle;}
	f3dPoint MirPoint(const double* pos_coord)const;
	f3dPoint MirVector(const double* vec_coord)const;
	bool IsSameMirMsg(MIRMSG &mirmsg);
};

XHLDSLM_API f3dPoint GetMirVector(f3dPoint src_vec,MIRMSG mirmsg);
XHLDSLM_API f3dPoint GetMirPos(f3dPoint src_pos,MIRMSG mirmsg);
//��������������жϵ���������λ��
XHLDSLM_API int CalPointQuadPos(f3dPoint pt);
XHLDSLM_API char CalPoleQuad(f3dPoint start,f3dPoint end);
//����ƽ���ϵ��ĵ��������ƽ��ķ��ߣ�����ʹ�ĵ�ӽ�ƽ�棩
XHLDSLM_API f3dPoint CalFaceNorm(f3dPoint p1,f3dPoint p2,f3dPoint p3,f3dPoint p4);
//ͼ�κ�����
XHLDSLM_API f3dPoint TransToUcsP(const UCS_STRU& ucs,const f3dPoint& pt);
XHLDSLM_API f3dPoint TransFromUcsP(const UCS_STRU& ucs, const f3dPoint &pt);
XHLDSLM_API f3dPoint TransToUcsV(const UCS_STRU& ucs,const f3dPoint& v);
XHLDSLM_API f3dPoint TransFromUcsV(const UCS_STRU& ucs, const f3dPoint &v);
XHLDSLM_API GEPOINT TransToUcsP(const UCS_STRU& ucs,const GEPOINT& pt);
XHLDSLM_API GEPOINT TransFromUcsP(const UCS_STRU& ucs, const GEPOINT &pt);
XHLDSLM_API GEPOINT TransToUcsV(const UCS_STRU& ucs,const GEPOINT& v);
XHLDSLM_API GEPOINT TransFromUcsV(const UCS_STRU& ucs, const GEPOINT &v);
XHLDSLM_API bool Standarized(double* v);
XHLDSLM_API UCS_STRU CalLocalCS(const f3dPoint& start,const f3dPoint& end);
//-------------VVV--�汾��������֤��������--VVV-------------------
XHLDSLM_API DWORD LdsLmDllGetVersion();
XHLDSLM_API bool ValidateLdsLmVersionCompatible(char* errmsg=NULL,int nMsgStrBuffLen=50);

#endif
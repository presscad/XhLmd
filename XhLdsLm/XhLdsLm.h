#ifndef __XH_LDS_LM_H_
#define __XH_LDS_LM_H_
// 下列 ifdef 块是创建使从 DLL 导出更简单的
//宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 XHLDSLM_EXPORTS
// 符号编译的。在使用此 DLL 的
//任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将 
// XHLDSLM_API 函数视为是从此 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef APP_EMBEDDED_MODULE_ENCRYPT
#define XHLDSLM_API	//内嵌时XHLDSLM_API什么都不代表
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
	BOOL And(CFGWORD wcfg) const;	//相当于用‘&’号判断是否有交集
	CFGWORD operator &(CFGWORD wcfg) const;
	CFGWORD operator &=(CFGWORD wcfg);
	CFGWORD ToBodyWord(BYTE schema=0) const;	//规则化为塔身本体的配材号（即补齐有效呼高的所有接腿位）
	BOOL IsHasNo(int iNo);			//配材字中是否含有指定的iNo号位
	BOOL IsHasBodyNoOnly(int iBodyNo,BYTE schema=0);	//配材字中是否仅指定字节是有值，即不为0，iByte以1为基数(即索引值+1)
	BOOL IsHasBodyNo(int iBodyNo,BYTE schema=0);		//配材字中指定字节是否有值，即不为0，iByte以1为基数(即索引值+1)
	BOOL AddBodyLegs(int iBodyNo,DWORD legword=0xffffff,BYTE schema=0);
	BOOL SetBodyLegs(int iBodyNo,DWORD legword=0xffffff,BYTE schema=0);
	DWORD GetBodyLegs(int iBodyNo,BYTE schema=0);
	BOOL IsEqual(CFGWORD cfgword,bool bVerifyBodyNo=false,BYTE schema=0);	//bVerifyBodyNo=true,时只验证对应字节是否都有值或无值
	BOOL IsNull(){return flag.word[0]==0&&flag.word[1]==0&&flag.word[2]==0&&flag.word[3]==0&&flag.word[4]==0&&flag.word[5]==0;}
	void SetWord(CFGWORD cfgword);				//直接指定配材字
	CFGWORD SetWordByNo(int iNo);			//根据指定的iNo号位指定配材字
	CFGWORD AddSpecNoWord(int iNo);			//在配材字中添加指定的号位
	CFGWORD RemoveSpecNoWord(int iNo);		//在配材字中移除指定的号位
	CFGWORD AddSpecWord(CFGWORD cfgword);	//相当于两个配材字进行或操作,并将结果赋值给当前配材字
	CFGWORD RemoveSpecWord(CFGWORD cfgword);//从当前配材字中移除与指定配材字共有的交集号位
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
	static BYTE MULTILEG_SCHEMA;	//当前默认的呼高接腿占位分配模式
public:	//多呼高多接腿的占位分配模式
	static const BYTE MULTILEG_DEFAULT	= 0;
	static const BYTE MULTILEG_MAX08	= 1;
	static const BYTE MULTILEG_MAX16	= 2;
	static const BYTE MULTILEG_MAX24	= 3;
	static BYTE Schema(){return MULTILEG_SCHEMA;}
	static BYTE SetSchema(BYTE cbMultiLegSchema);
	static BYTE ValidateSchema(BYTE schema);	//确保返回模式为合法呼高接腿占位分配模式
	static BYTE MaxLegs(BYTE schema=MULTILEG_DEFAULT);	//指定模式支持最多呼高接腿数
	static BYTE MaxBodys(BYTE schema=MULTILEG_DEFAULT);	//指定模式支持最多呼高本体数
public:
	CFGLEG();	//默认1A接腿
	CFGLEG(DWORD flag);	//默认1A接腿
	CFGLEG(BYTE iBodyNo,BYTE iLegNo,BYTE schema=MULTILEG_DEFAULT);
	CFGLEG ConvertSchema(BYTE schemaFrom,BYTE schemaTo,char* sError=NULL,UINT uStrBufLen=0) const;
	CFGWORD ToCfgWord(BYTE schema=MULTILEG_DEFAULT);
	operator DWORD(){return code.flag;}
	DWORD BodyFlag(BYTE schema=MULTILEG_DEFAULT) const;
	DWORD LegFlag (BYTE schema=MULTILEG_DEFAULT) const;
	bool IsHasNo(int iBodyNo,int iLegNo=1,bool bVerifyLegNo=false,BYTE schema=MULTILEG_DEFAULT);
	CFGLEG AddBodyNo(int iBodyNo,BYTE schema=MULTILEG_DEFAULT);
	CFGLEG AddLegNo(int iLegNo,BYTE schema=MULTILEG_DEFAULT);
	CFGLEG AddSpecWord(CFGLEG bodyleg);	//相当于两个配材字进行或操作,并将结果赋值给当前配材字
	CFGLEG RemoveSpecNoWord(int iNo,bool bLegNo,BYTE schema=MULTILEG_DEFAULT);
	CFGLEG SetBodyFlag(DWORD bodyFlag,BYTE schema=MULTILEG_DEFAULT);
	CFGLEG SetLegFlag(DWORD legFlag,BYTE schema=MULTILEG_DEFAULT);
	int GetBodyScopeStr(char* scopeStr,int maxCount=51,BYTE schema=MULTILEG_DEFAULT);
	int GetLegScopeStr(char* scopeStr,int maxCount=17,BYTE schema=MULTILEG_DEFAULT);
};

XHLDSLM_API int ImportLicFile(char* licfile,BYTE cProductType,DWORD version[2]);
//由序号形式变为CFGWORD类型配材号对象
XHLDSLM_API CFGWORD GetCfgWord(long iNo);	//从1开始计数
XHLDSLM_API int MakeByteToCharStr(BYTE ch,char* cfg_str,char baseChar='A');
XHLDSLM_API int MakeBytesToString(BYTE* byteArr,int bytes_size,char *cfg_str,int max_buf_len);//将多个字节位转换为字符串形式的配材号
XHLDSLM_API int MakeWordToString(DWORD dwPartNo,char *cfg_str); //将DWORD 转换为字符串形式的配材号
XHLDSLM_API int MakeWordToString(CFGWORD dwCfgPartNo,char *cfg_str); //将CFGWORD 转换为字符串形式的配材号
XHLDSLM_API bool SimplifiedNumString(char *num_str);
//radius:圆弧半径；sector_angle:圆弧扇形角；scale_of_user2scr:实际与屏幕显示的尺寸比例；sample_len屏幕显示圆弧的抽样长度；smoothness圆柱面最低显示片数
XHLDSLM_API int CalArcResolution(double radius,double sector_angle,double scale_of_user2scr,double sample_len,int smoothness=36);
//输入精确板厚，返回工程中相接近的标准化钢板厚
XHLDSLM_API int FindPlankThick(double f_thick);
XHLDSLM_API BOOL IsMatch(char ch, char charSet[]);//判断cahrSet中是否有与ch相匹配的字符
XHLDSLM_API double CalSteelWeight(double vol);

#include "f_ent.h"

class XHLDSLM_API MIRMSG{
public:
	BYTE axis_flag;		//0x01:X轴;0x02:Y轴;0x04:Z轴;0x08:旋转对称;0x10:平面镜像
	BYTE array_num;		//旋转对称的次数
	short rotate_angle;	//绕旋转轴(Z轴)旋转角度(°角度制)
	GEPOINT origin,mir_norm;	//对称基点，旋转轴方向或镜像对称平面法线
public:
	MIRMSG();
	MIRMSG(int mir_x1_y2_z3);
	MIRMSG GetSubMirItem(int mir_x1_y2_z3_p4);	//输入参数4时表示镜面对称(相对平面mir_norm)
	short ValidRotAngle(){return (axis_flag&0x08)==0 ? 0 : rotate_angle;}
	f3dPoint MirPoint(const double* pos_coord)const;
	f3dPoint MirVector(const double* vec_coord)const;
	bool IsSameMirMsg(MIRMSG &mirmsg);
};

XHLDSLM_API f3dPoint GetMirVector(f3dPoint src_vec,MIRMSG mirmsg);
XHLDSLM_API f3dPoint GetMirPos(f3dPoint src_pos,MIRMSG mirmsg);
//根据输入点坐标判断点所在象限位置
XHLDSLM_API int CalPointQuadPos(f3dPoint pt);
XHLDSLM_API char CalPoleQuad(f3dPoint start,f3dPoint end);
//根据平面上的四点来求近似平面的法线（尽量使四点接近平面）
XHLDSLM_API f3dPoint CalFaceNorm(f3dPoint p1,f3dPoint p2,f3dPoint p3,f3dPoint p4);
//图形函数区
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
//-------------VVV--版本兼容性验证函数声明--VVV-------------------
XHLDSLM_API DWORD LdsLmDllGetVersion();
XHLDSLM_API bool ValidateLdsLmVersionCompatible(char* errmsg=NULL,int nMsgStrBuffLen=50);

#endif
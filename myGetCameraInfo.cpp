#include "iostream" 
#include "opencv2/opencv.hpp"
#include <atlbase.h>
#include "strmif.h"
#include "uuids.h"
#include "setupapi.h"
#include <initguid.h>

#pragma comment(lib,"setupapi.lib")
#pragma comment(lib,"Strmiids.lib")
using namespace cv;
using namespace std;

void DevicesInfo()
{
    // 得到所有设备 HDEVINFO    
    HDEVINFO hDevInfo;
    hDevInfo = SetupDiGetClassDevs(NULL, 0, 0, DIGCF_PRESENT | DIGCF_ALLCLASSES);
    if (hDevInfo == INVALID_HANDLE_VALUE)
    {
        wprintf(L"SetupDiGetClassDevs Err:%d", GetLastError());
    }

    // 循环列举   
    SP_DEVINFO_DATA DeviceInfoData;
    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); i++)
    {
        TCHAR szClassBuf[MAX_PATH] = { 0 };
        TCHAR szDescBuf[MAX_PATH] = { 0 };
        TCHAR hardWareid[MAX_PATH] = { 0 };
        TCHAR friendName[MAX_PATH] = { 0 };
        // 获取类名  
        SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData, SPDRP_CLASS, NULL, (PBYTE)szClassBuf, MAX_PATH - 1, NULL);
        //获取设备描述信息
        SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData, SPDRP_DEVICEDESC, NULL, (PBYTE)szDescBuf, MAX_PATH - 1, NULL);
        //获取硬件ID  
        SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData, SPDRP_HARDWAREID, NULL, (PBYTE)hardWareid, sizeof(hardWareid), NULL);
        //获取友好名称
        SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData, SPDRP_FRIENDLYNAME, NULL, (PBYTE)friendName, sizeof(friendName), NULL);

        string Class = szClassBuf;
        if (Class == "Camera")
        {
            string Name = friendName;
            string ID = hardWareid;
            string Vid = ID.substr(ID.find("VID_") + 4, 4);
            string Pid = ID.substr(ID.find("PID_") + 4, 4);
            printf("CameraName:%s\r\n", Name.c_str());
            printf("ID:%s\r\n", ID.c_str());
            printf("Vid:%s\r\n", Vid.c_str());
            printf("Pid:%s\r\n", Pid.c_str());                
        }
    }
    SetupDiDestroyDeviceInfoList(hDevInfo);
}

int EnumDevices(vector<string>& CameraName, vector<string>& CameraVidPid)
{
    int ret = CoInitialize(nullptr);
    int nCount = 0;
    CameraName.clear();
    CameraVidPid.clear();
    //第一步：创建一个系统枚举器组件对象，同时查询一个指定了CLSID的一个接口。
    CComPtr<ICreateDevEnum> pCreateDevEnum;
    HRESULT hr = CoCreateInstance(
        CLSID_SystemDeviceEnum,//用来唯一标识一个对象的CLSID（128位），需要它用来创建指定对象
        nullptr,//指向接口IUnknown的指针
        CLSCTX_INPROC_SERVER,//预先可执行代码的上下文,[in]组件类别，可使用CLSCTX枚举器中预定义值
        IID_ICreateDevEnum,//创建的Com对象的接口标识符，EXTERN_C const IID_ICreateDevEnum
        (void**)&pCreateDevEnum//用来接收指向Com对象接口地址的变量指针
    );
    if (hr != NOERROR) 
        return -1;//创建设备枚举器（Enumerator）失败

    //第二步：为指定的Filter注册类型创建一个枚举器，获得IEnumMoniker接口
    CComPtr<IEnumMoniker> pEm;
    hr = pCreateDevEnum->CreateClassEnumerator(
        CLSID_VideoInputDeviceCategory,
        &pEm,
        0);
    if (hr != NOERROR) return -1;
    pEm->Reset();
    //第三步：使用IEnumMoniker接口枚举所有的设备标识
    ULONG cFetched;
    IMoniker* pM = nullptr;
    while (hr = pEm->Next(1, &pM, &cFetched), hr == S_OK)
    {
        IPropertyBag* pBag = nullptr;
        hr = pM->BindToStorage(nullptr, nullptr, IID_IPropertyBag, (void**)&pBag);
        if (SUCCEEDED(hr))
        {
            VARIANT var;
            var.vt = VT_BSTR;
                  
            hr = pBag->Read(L"FriendlyName", &var, nullptr);
            if (hr == NOERROR)
            {          
                char tmp[2048];
                WideCharToMultiByte(CP_ACP, 0, var.bstrVal, -1, tmp, 2048, nullptr, nullptr);
                CameraName.push_back(tmp);
                SysFreeString(var.bstrVal);
                cout << tmp << endl;
            }
            hr = pBag->Read(L"DevicePath", &var, nullptr);
            if (hr == NOERROR)
            {
                char path[2048];
                WideCharToMultiByte(CP_ACP, 0, var.bstrVal, -1, path, 2048, nullptr, nullptr);
                std::string tmp = path;
                tmp = tmp.substr(tmp.find("vid_") + 4, 4) + tmp.substr(tmp.find("pid_") + 4, 4);
                CameraVidPid.push_back(tmp);
                SysFreeString(var.bstrVal);
                cout << tmp << endl;
            }
            pBag->Release();
        }
        pM->Release();
    }
    pCreateDevEnum = nullptr;
    pEm = nullptr;
    CoUninitialize();
    return nCount;
}

int main()
{
    printf("-----------------DevicesInfo-----------------\r\n");
    DevicesInfo();

    printf("-----------------EnumDevices-----------------\r\n");
    vector<string> Name;
    vector<string> VidPid;
    EnumDevices(Name, VidPid);
    system("pause");
    return 0;
}

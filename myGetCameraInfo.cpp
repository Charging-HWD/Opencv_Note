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
    // �õ������豸 HDEVINFO    
    HDEVINFO hDevInfo;
    hDevInfo = SetupDiGetClassDevs(NULL, 0, 0, DIGCF_PRESENT | DIGCF_ALLCLASSES);
    if (hDevInfo == INVALID_HANDLE_VALUE)
    {
        wprintf(L"SetupDiGetClassDevs Err:%d", GetLastError());
    }

    // ѭ���о�   
    SP_DEVINFO_DATA DeviceInfoData;
    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); i++)
    {
        TCHAR szClassBuf[MAX_PATH] = { 0 };
        TCHAR szDescBuf[MAX_PATH] = { 0 };
        TCHAR hardWareid[MAX_PATH] = { 0 };
        TCHAR friendName[MAX_PATH] = { 0 };
        // ��ȡ����  
        SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData, SPDRP_CLASS, NULL, (PBYTE)szClassBuf, MAX_PATH - 1, NULL);
        //��ȡ�豸������Ϣ
        SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData, SPDRP_DEVICEDESC, NULL, (PBYTE)szDescBuf, MAX_PATH - 1, NULL);
        //��ȡӲ��ID  
        SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData, SPDRP_HARDWAREID, NULL, (PBYTE)hardWareid, sizeof(hardWareid), NULL);
        //��ȡ�Ѻ�����
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
    //��һ��������һ��ϵͳö�����������ͬʱ��ѯһ��ָ����CLSID��һ���ӿڡ�
    CComPtr<ICreateDevEnum> pCreateDevEnum;
    HRESULT hr = CoCreateInstance(
        CLSID_SystemDeviceEnum,//����Ψһ��ʶһ�������CLSID��128λ������Ҫ����������ָ������
        nullptr,//ָ��ӿ�IUnknown��ָ��
        CLSCTX_INPROC_SERVER,//Ԥ�ȿ�ִ�д����������,[in]�����𣬿�ʹ��CLSCTXö������Ԥ����ֵ
        IID_ICreateDevEnum,//������Com����Ľӿڱ�ʶ����EXTERN_C const IID_ICreateDevEnum
        (void**)&pCreateDevEnum//��������ָ��Com����ӿڵ�ַ�ı���ָ��
    );
    if (hr != NOERROR) 
        return -1;//�����豸ö������Enumerator��ʧ��

    //�ڶ�����Ϊָ����Filterע�����ʹ���һ��ö���������IEnumMoniker�ӿ�
    CComPtr<IEnumMoniker> pEm;
    hr = pCreateDevEnum->CreateClassEnumerator(
        CLSID_VideoInputDeviceCategory,
        &pEm,
        0);
    if (hr != NOERROR) return -1;
    pEm->Reset();
    //��������ʹ��IEnumMoniker�ӿ�ö�����е��豸��ʶ
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

##这是一个蓝牙遥控器的产测工具， 可以实现自动连接。
##MFC实现， 以下包含用到的技术

1. 检查目录是否存在， 创建目录
2. 检查文件是否存在
3. 三种类型的Dll， http://www.cnblogs.com/duanshuiliu/archive/2012/07/05/2577402.html
   Dll中导出函数时， 最好用extern "C" __declspec(dllexport) void Func()形式， 
   即以C的函数符号名导出该函数， 如果不加的话就会以C++的形式给出， 比较复杂
   查看Dll中导出函数， 用VS Tools->Visual studio 2008 Command Prompt, d:\dumpbin /exports a.dll
4. 动态加载dll， 用AfxLoadLibrary(DecoderFilePath)
   获得dll中的函数， m_DecoderFunc = (AUDIO_DECODER)GetProcAddress(hDll,"AudioDecoder");
5. CStringArray
6. 遍历目录下的文件， CFileFind
7. GetCurrentDirectory, GetModuleFileName
   GetCurrentDirectory, 只是获取当前的目录，当计算机重新启动后，当前的路径一般为系统目录，
   因此系统启动后，使用该函数只能获取当前的目录，而不是应用程序所在的目录。
8. TODO： 从遥控器端读到Voice使用哪种编码！！！
9. cstring类型不能向dll传递 请转换为char* 或lpctstr或BSTR
http://blog.csdn.net/wangqiulin123456/article/details/8114638

用const CString &Path 是可以的！！！， 最好还是用BSTR

CString <==> LPTSTR
http://blog.sina.com.cn/s/blog_5d2412000100om0p.html
10. AES256:     http://blog.csdn.net/searchsun/article/details/2516191
11. Wave文件    http://blog.csdn.net/sshcx/article/details/1593923
12. PlaySound   https://msdn.microsoft.com/en-us/library/dd743680%28VS.85%29.aspx
需要加入头文件Mmsystem.h (include Windows.h)
加入库Winmm.lib
13. XCOPY   
    http://www.ynpxrz.com/n614941c2025.aspx
    http://www.jb51.net/article/29055.htm
14.  使用MFC的API函数获取Window下的特殊目录(例如CSIDL_PERSONAL 我的文档) 
    http://blog.csdn.net/chunyexiyu/article/details/10341139
    http://www.cnblogs.com/Ray-chen/archive/2011/11/14/2248496.html

15. dbport=GetPrivateProfileInt("database","port",143,".\\dbsettings.ini");
GetPrivateProfileString

一、向ini文件中写入信息的函数
1. 把信息写入系统的win.ini文件
BOOL WriteProfileString(
      LPCTSTR lpAppName, // 节的名字，是一个以0结束的字符串
      LPCTSTR lpKeyName, // 键的名字，是一个以0结束的字符串。若为NULL，则删除整个节
      LPCTSTR lpString       // 键的值，是一个以0结束的字符串。若为NULL，则删除对应的键
)

2. 把信息写入自己定义的.ini文件
BOOL WritePrivateProfileString(
      LPCTSTR lpAppName,      // 同上
      LPCTSTR lpKeyName,      // 同上
      LPCTSTR lpString,       // 同上
      LPCTSTR lpFileName      // 要写入的文件的文件名。若该ini文件与程序在同一个目录下，也可使用相对
            //路径,否则需要给出绝度路径。
)

16. SBC  ->  mSBC
每个Frame中可能包含两笔包， 例如120的Frame Size， 可能包含两个SBC frame
每个SBC frame在解码时要按照它编码时的参数进行， 这个参数保存在frame header中

在播放声音之前， 要设置好WAVEFormatex结构， 要和实际播放的声音数据对应， 否则会是噪音！！！
    /*
	* set up the WAVEFORMATEX structure.
	*/
	wfx.nSamplesPerSec = 16000; /* sample rate */
	wfx.wBitsPerSample = 16; /* sample size */
	wfx.nChannels = 1; /* channels*/
	wfx.cbSize = 0; /* size of _extra_ info */
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nBlockAlign = (wfx.wBitsPerSample * wfx.nChannels) >> 3;
	wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;

17. map
Map中的元素是自动按key升序排序
使用枚举器查找或删除指定元素:
1)map中元素的查找：
   find()函数返回一个迭代器指向键值为key的元素，如果没找到就返回指向map尾部的迭代器。        
   map<int ,string >::iterator l_it;; 
   l_it=maplive.find(112);
   if(l_it==maplive.end())
        cout<<"we do not find 112"<<endl;
   else cout<<"wo find 112"<<endl;
2)map中元素的删除：
   如果删除112；
   map<int ,string >::iterator l_it;;
   l_it=maplive.find(112);
   if(l_it==maplive.end())
        cout<<"we do not find 112"<<endl;
   else  maplive.erase(l_it);  //delete 112;

18. CString  SpanIncluding
使用CString::SpanIncluding可以检验字符串的合法性，是否是我们要求的字符，比如全是数字构成的字符串，我们可以用下面的代码进行判断：
http://blog.csdn.net/chenlycly/article/details/19937111

19. CEdit 自定义控件
http://blog.csdn.net/anye3000/article/details/8529669

20. 字符串分割
http://yq1518602434.blog.163.com/blog/static/202937084201302741030311/

21. Button添加图片
1) ICON
按钮的Icon属性设为true，然后在OnInitDialog中
((CButton *)GetDlgItem(IDC_BUTTON))->SetIcon(AfxGetApp()->LoadIcon(IDI_ICON));
2) BMP
按钮的Bitmap属性设为true，然后在OnInitDialog中
http://blog.csdn.net/lightboat09/article/details/6069710

22. CButtonST
http://blog.csdn.net/liuyang1943/article/details/4959454

23. ShellExecute
https://msdn.microsoft.com/en-us/library/windows/desktop/bb762153%28v=vs.85%29.aspx

24. CRegKey
路径格式： #define SERVICE_LE_UI_PATCH     L"Software\\Realtek\\"       前面不能有\\， 后面必须有\\
LeDeviceKey.DeleteValue(CString(L"Ediv"));  // Delete Value
LeDeviceKey.DeleteSubKey(m_BDAddrStr);	// only delete empty subkey
LeDeviceKey.RecurseDeleteKey(m_BDAddrStr);	// can delete non-empty subkey

25. 注册表操作
http://www.cnblogs.com/trying/archive/2012/07/16/2863763.html

26. VC的Picture Control控件加载图片BMP、PNG的方法
http://dragoniye.com/archives/57

27. mfc获取控件在对话框上的位置
    CRect rect;
    GetDlgItem(控件ID)->GetWindowRect(&rect);//获取控件的屏幕坐标
    ScreenToClient(&rect);//转换为对话框上的客户坐标

28. 在Dialog中嵌入Doc-View
    Create Doc and Create View, 可先指定文件， 之后文件更新时再更新View
    Doc指的是文件， View是一个Win， 可以在上面作图
29. 加载图片， 如果使用动态方式直接打开图片， 需要将图片也放入程序所在位置
    如果将图片加入到资源中， 不再需要另存图片了
30. MFC键盘响应函数
    BOOL CHWCDlg::PreTranslateMessage(MSG* pMsg)
    {
        if(pMsg->message == WM_KEYDOWN)
        {
            switch(pMsg->wParam)
            {
            case VK_LEFT:
                MessageBox(L"Left");
                break;
            case VK_RIGHT:
                MessageBox(L"Right");
                break;
            case VK_UP:
                MessageBox(L"Up");
                break;
            case VK_DOWN:
                MessageBox(L"Down");
                break;
            }
        }
        return CDialogEx::PreTranslateMessage(pMsg);
    }
    遇到的问题， 在UI App打开之后， 按下一个Mode Button，这时按下键盘键， 该函数不能响应，只有再按下任意Button时才可以响应。
    原因是在Mode Button处理函数时将Button的Disable了， 不Disable就不会有上述现象。

31. Create Thread to do things
    UINT WINAPI MyThreadProc(LPVOID pParam)
    {
        CString SBC_WavePath;
        SBC_WavePath = VoiceDir + SBC_WAVE_PATH;
        PlaySound(SBC_WavePath, NULL, SND_FILENAME);
        return 1;
    }

    extern "C"  SBC_HAIXIN_API int AudioPlay(void)
    {
        CWinThread *pThread = AfxBeginThread((AFX_THREADPROC)MyThreadProc,NULL);
        return 0;
    }

32. 序列化与反序列化CArchive
序列化 (Serialization)将对象的状态信息转换为可以存储或传输的形式的过程。在序列化期间，对象将其当前状态写入到临时或持久性存储区。以后，可以通过从存储区中读取或反序列化对象的状态，重新创建该对象。
主要用于存储对象状态为另一种通用格式，比如存储为二进制、xml、json等等，把对象转换成这种格式就叫序列化，而反序列化通常是从这种格式转换回来。
使用序列化主要是因为跨平台和对象存储的需求，因为网络上只允许字符串或者二进制格式，而文件需要使用二进制流格式，如果想把一个内存中的对象存储下来就必须使用序列化转换为xml（字符串）、json（字符串）或二进制（流） 

CArchive 对象提供了一个类型安全缓冲机制，用于将可序列化对象写入 CFile 对象或从中读取可序列化对象。
给定的 CArchive 对象要么存储数据（即写入数据或将数据序列化），要么加载数据（即读取数据或将数据反序列化），但决不能同时进行。CArchive 对象的寿命只限于将对象写入文件或从文件读取对象的一次传递。
http://www.cnblogs.com/ylhome/archive/2010/03/15/1686558.html

C:          序列化与反序列化库  https://github.com/troydhanson/tpl
Python:     Python序列化与反序列化Module,  Pickle、CPickle
C#:         .NET框架提供了两种种串行化的方式：1、是使用BinaryFormatter进行串行化；2、使用XmlSerializer进行串行化。
            第一种方式提供了一个简单的二进制数据流以及某些附加的类型信息，而第二种将数据流格式化为XML存储。   



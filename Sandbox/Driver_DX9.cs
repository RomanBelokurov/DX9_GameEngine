using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Sandbox
{
    class Driver_DX9
    {
        [DllImport("DXEngine.dll", CallingConvention = CallingConvention.Cdecl)]
        static public extern IntPtr GetInstance();


        [DllImport("DXEngine.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool InitDriver(IntPtr pClassNameObject, IntPtr hWnd);


        [DllImport("DXEngine.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void Begin(IntPtr pClassNameObject, UInt32 param);


        [DllImport("DXEngine.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void End(IntPtr pClassNameObject);


    }
}

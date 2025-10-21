using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;

namespace Emergent.Gamebryo.SceneDesigner.GUI.Utility
{
	/// <summary>
	/// This class contains definitions for native Win32 calls
	/// </summary>
	public class NativeWin32
	{
		public NativeWin32()
		{
		}
		
		#region Constants
		internal const int MAX_PATH = 260;
		#endregion

		#region Types

		#endregion

		#region Kernal32 Win32API

		[DllImport("kernel32.dll", SetLastError=true, CharSet=CharSet.Auto)]
		internal static extern uint GetLongPathName(
			string lpszShortPath,
			[Out] StringBuilder lpszLongPath,
			uint cchBuffer);	
		#endregion

		#region User32 Win32API

		[DllImport("user32.dll", SetLastError=true, CharSet=CharSet.Auto)]
		internal static extern IntPtr SetActiveWindow(IntPtr hwnd);

		#endregion
	}

}

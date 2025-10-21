// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2006 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

using System;
using System.Windows.Forms;
using Microsoft.Win32;

namespace AnimationToolUI
{
	/// <summary>
	/// Summary description for AppUtils.
	/// </summary>
	public class AppUtils
	{
		static public string ExtractDirFromPath(string strAbsolutePath)
        {
            string delimStr = "\\/";
            char [] delimiter = delimStr.ToCharArray();
            string [] splitAbsolute = strAbsolutePath.Split(delimiter);
            string strDir = "";

            for (int i = 0; i < splitAbsolute.Length - 1; i++)
            {
                strDir += splitAbsolute[i] + '\\';
            }
            return strDir;
        }

        static public bool EqualsNoCase(string strA, string strB)
        {
            strA = strA.ToUpper();
            strB = strB.ToUpper();
            return (strA.CompareTo(strB) == 0);
        }

        static public string ExtractFileFromPath(string strAbsolutePath)
        {
            string delimStr = "\\/";
            char [] delimiter = delimStr.ToCharArray();
            string [] splitAbsolute = strAbsolutePath.Split(delimiter);
            string strFile = splitAbsolute[splitAbsolute.Length - 1];
            return strFile;
        }

        static public string CreateRelativePath(string strAbsolutePath,
            string strRelativeToPath, bool bBackSlash)
        {
            strAbsolutePath = strAbsolutePath.ToLower();
            strRelativeToPath = strRelativeToPath.ToLower();
            string delimStr = "\\/";
            char [] delimiter = delimStr.ToCharArray();
            string [] splitAbsolute = null;
            string [] splitRelativeToPath = null;

            splitAbsolute = strAbsolutePath.Split(delimiter);
            splitRelativeToPath = strRelativeToPath.Split(delimiter);
            string strCommonPath = new string(' ', 1);
            strCommonPath.Trim();
            string strRelativePath = strCommonPath;

            int iAbsolute = 0;
            int iRelativeTo = 0;
			while (iAbsolute < splitAbsolute.Length && iRelativeTo < splitRelativeToPath.Length)
            {
                if (splitAbsolute[iAbsolute].Length == 0)
                {
                    iAbsolute++;
                    continue;
                }

                if (splitRelativeToPath[iRelativeTo].Length == 0)
                {
                    iRelativeTo++;
                    continue;
                }

                if (splitAbsolute[iAbsolute] == splitRelativeToPath[iRelativeTo])
                {
                    strCommonPath += splitAbsolute[iAbsolute] + '\\';
                    iRelativeTo++;
                    iAbsolute++;
                }
                else
                {
                    break;
                }
            }

            // If the drive isn't even the same, return the full 
            // path
            if (iAbsolute == 0 || iRelativeTo == 0)
                return strAbsolutePath;
            
            for (int i = iRelativeTo; i < splitRelativeToPath.Length; i++)
            {
                if (splitRelativeToPath[i].Length == 0)
                    continue;

                strRelativePath += ".." + (bBackSlash ? '\\' : '/');
            }

            for (int i = iAbsolute; i < splitAbsolute.Length - 1; i++)
            {
                if (splitAbsolute[i].Length == 0)
                    continue;

				strRelativePath += splitAbsolute[i] + (bBackSlash ? '\\' : '/');
            }

            strRelativePath += splitAbsolute[splitAbsolute.Length - 1];
            return strRelativePath.Trim();
        }

        [System.Runtime.InteropServices.DllImport("kernel32.dll")]
        static extern uint GetLongPathName(string strShortName,
            System.Text.StringBuilder sbLongNameBuff, uint uiBufferSize);

        public static string ToLongPathName(string strShortName)
        {
            System.Text.StringBuilder sbLongNameBuffer =
                new System.Text.StringBuilder(512);
            uint uiBufferSize = (uint) sbLongNameBuffer.Capacity;

            GetLongPathName(strShortName, sbLongNameBuffer, uiBufferSize);

            return sbLongNameBuffer.ToString();
        }

		static public void StoreRegKey(string kRegKey, string kValue)
		{
			// ItemRootPath를 Registry에 저장한다.
			string kRegistryPath = "Software\\" + Application.CompanyName + "\\" +
				Application.ProductName;

			RegistryKey key = Registry.CurrentUser.CreateSubKey(
				kRegistryPath + "\\ItemSetting");

			key.SetValue(kRegKey, kValue);
		}

		static public bool RestoreRegKey(string kRegKey, ref string rkOutValue)
		{
			// Registry에 있는 ItemRootPath를 가져온다.
			string kRegistryPath = "Software\\" + Application.CompanyName + "\\" +
				Application.ProductName;

			RegistryKey key = Registry.CurrentUser.OpenSubKey(
			   kRegistryPath + "\\ItemSetting");

			if (key == null)
			{
				return false;
			}

			rkOutValue = (string)key.GetValue(kRegKey, string.Empty);
			return true;
		}
	}
}

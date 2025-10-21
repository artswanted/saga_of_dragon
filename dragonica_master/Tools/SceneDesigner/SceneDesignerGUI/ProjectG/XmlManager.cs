using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;
using System.IO;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

namespace Emergent.Gamebryo.SceneDesigner.GUI.ProjectG
{
	public struct XML_LOCATION
	{
		public float fLocX;
		public float fLocY;
		public float fLocZ;

		public XML_LOCATION StringsToLocation(string str)
		{
			XML_LOCATION loc = new XML_LOCATION();

			// Location ³ª´²¼­ ³Ö´Â´Ù.
			char[] szDest = new char[256];
			int iDot = str.IndexOf(",", 0);
			str.CopyTo(0, szDest, 0, iDot);
			string strDest = null;
			foreach (char c in szDest)
				if (c.ToString() != "\0")
					strDest += c.ToString();
			loc.fLocX = float.Parse(strDest);

			szDest = new char[256];
			int iDot2 = str.IndexOf(",", iDot + 1);
			str.CopyTo(iDot + 1, szDest, 0, (iDot2) - (iDot + 1));
			strDest = null;
			foreach (char c in szDest)
				if (c.ToString() != "\0")
					strDest += c.ToString();
			loc.fLocY = float.Parse(strDest);

			szDest = new char[256];
			iDot = iDot2;
			iDot2 = str.Length;
			str.CopyTo(iDot + 1, szDest, 0, (iDot2) - (iDot + 1));
			strDest = null;
			foreach (char c in szDest)
				if (c.ToString() != "\0")
					strDest += c.ToString();
			loc.fLocZ = float.Parse(strDest);

			return loc;
		}
	}

	public class XmlManager
	{
		private XmlDocument m_Document = new XmlDocument();
		DirectoryInfo m_diInfo = null;
		string m_strFilename = null;

		public XmlManager()
		{
			XmlDeclaration newDec = m_Document.CreateXmlDeclaration("1.0", "euc-kr", null);
			m_Document.AppendChild(newDec);

			XmlElement newRoot = m_Document.CreateElement("WORLD");
			newRoot.SetAttribute("ID", "none");
			m_Document.AppendChild(newRoot);

			XmlNode pNode = m_Document.CreateNode(XmlNodeType.Element, "GSAPATH", null);
			pNode.InnerText = "";
			newRoot.AppendChild(pNode);

			pNode = m_Document.CreateNode(XmlNodeType.Element, "SCRIPT", null);
			pNode.InnerText = "";
			newRoot.AppendChild(pNode);

			XmlElement pElement = m_Document.CreateElement("BGSOUND");
			pElement.InnerText = "";
			pElement.SetAttribute("VOLUME", "1.0");
			newRoot.AppendChild(pElement);

			pElement = m_Document.CreateElement("BLOOM");
			pElement.SetAttribute("BLUR", "1.5");
			pElement.SetAttribute("BRIGHT", "10.0");
			pElement.SetAttribute("SRC", "1.65");
			pElement.SetAttribute("GLOW", "1.45");
			pElement.SetAttribute("HIGHRIGHT", "1.0");
			newRoot.AppendChild(pElement);

			XmlComment pComment = m_Document.CreateComment(" TYPE - ÀÏ¹Ý¸Ê: 1, ÀÎ´ø¸Ê: 2, ¸¶À»¸Ê: 8, ¸¶ÀÌ·ë: 16, PVP_KTH: 32, PVP_DM: 64, ¹Ì¼Ç¸Ê: 128 ");
			newRoot.AppendChild(pComment);

			pElement = m_Document.CreateElement("MAPVALUE");
			pElement.SetAttribute("PLAYERKILL", "0");
			pElement.SetAttribute("TYPE", "1");
			newRoot.AppendChild(pElement);
						
			ActionFileList("");
		}

		public void XmlFileLoad(string strFilename)
		{
			m_strFilename = strFilename;
            try
            {
                m_Document.Load(strFilename);
            }
            catch(Exception exp)
            {
                MessageBox.Show("Failed load xml file: " + strFilename + "\n\n" + exp.ToString());
            }
		}

		public string GetFilename()
		{
			return m_strFilename;
		}

		public XmlDocument GetDocument()
		{
			return m_Document;
		}

		public DirectoryInfo GetDirectoryInfo()
		{
			return m_diInfo;
		}

		public void ActionFileList(string strPath)
		{
			if (strPath.Length > 0)
			{
				m_diInfo = new DirectoryInfo(strPath);  //°Ë»ö µð·ºÅä¸®
			}
			//m_fiInfo = di.GetFiles("*.xml");
		}
	}
}

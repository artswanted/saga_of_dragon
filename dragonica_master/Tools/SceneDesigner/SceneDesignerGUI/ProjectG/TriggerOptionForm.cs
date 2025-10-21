using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Xml;

namespace Emergent.Gamebryo.SceneDesigner.GUI
{
	public partial class TriggerOptionForm : Form
	{
		string m_strTriggerID = null;
		string m_strConditionType = null;
		int m_iConditionTypeIdx=0;
		string m_strConditionActionType = null;
		int m_iConditionActionTypeIdx = 0;
		string m_strActionScript = null;
		string m_strParameter1 = null;
		string m_strParameter2 = null;
		XmlDocument m_XmlDocument;
		XmlElement m_pElement;

		int m_iConditionTypeCount=3;
		public string[] m_strConditionTypeList;// = {"action", "time", "touch"};
		public string[] m_strConditionActionTypeList;

		public TriggerOptionForm()
		{
			InitializeComponent();
		}

		// 기본 콤보박스의 내용을 채워 넣는다.
		private void Initialize(DirectoryInfo diInfo)
		{
			m_cbConditionType.Items.Clear();
			m_strConditionTypeList = new string[m_iConditionTypeCount];
			for (int i = 0; i < m_iConditionTypeCount; i++)
				switch(i)
				{
					case 0:
						m_strConditionTypeList[i] = "action";
						break;
					case 1:
						m_strConditionTypeList[i] = "time";
						break;
					case 2:
						m_strConditionTypeList[i] = "touch";
						break;
				}

			for (int i = 0; i < m_iConditionTypeCount; i++)
				m_cbConditionType.Items.Add(m_strConditionTypeList[i]);

			m_cbActionType.Items.Clear();

			// 다이얼로그에 표시될 액션 이름을 고쳐서 표시.

			if (diInfo != null && diInfo.Exists)
			{
				int j = 0;
				foreach (FileInfo fiInfo in diInfo.GetFiles())
				{
					j += 1;
				}

				m_strConditionActionTypeList = new string[j];
				j = 0;
				foreach (FileInfo fiInfo in diInfo.GetFiles())
				{
					String stDest = null;
					char[] szDest = new char[256];
					int iLastDot = fiInfo.Name.LastIndexOf(".");
					fiInfo.Name.CopyTo(0, szDest, 0, iLastDot);
					foreach (char c in szDest)
						if (c.ToString() != "\0")
							stDest += c.ToString();

					string strAction;
					strAction = "a_" + stDest;

					m_cbActionType.Items.Add(strAction);
					m_strConditionActionTypeList[j] = strAction;
					j += 1;
				}
			}
			else
			{
				m_cbActionType.Items.Add("a_up");
				m_strConditionActionTypeList = new string[1];
				m_strConditionActionTypeList[0] = "a_up";
			}

			m_cbConditionType.SelectedIndex = 0;
			m_cbActionType.SelectedIndex = 0;
		}

		public void SaveNode()
		{
			m_strTriggerID =  m_tbTriggerID.Text;
			if (m_cbConditionType.SelectedIndex != -1)
				m_strConditionType = m_strConditionTypeList[m_cbConditionType.SelectedIndex];
			else
				m_strConditionType = m_cbConditionType.Text;
			if (m_cbActionType.SelectedIndex != -1)
				m_strConditionActionType = m_strConditionActionTypeList[m_cbActionType.SelectedIndex];
			else
				m_strConditionActionType = m_cbActionType.Text;
			m_strActionScript = m_tbActionScript.Text;
			if (m_tbParameter1.Text != "")
				m_strParameter1 = m_tbParameter1.Text;
			else
				m_strParameter1 = null;
			if (m_tbParameter2.Text != "")
				m_strParameter2 = m_tbParameter2.Text;
			else
				m_strParameter2 = null;

			XmlDocument XmlRoot = m_XmlDocument;
			//m_pElement = XmlRoot.CreateElement("TRIGGER");
			m_pElement.SetAttribute("ID", m_strTriggerID);

			XmlElement XmlCondition = XmlRoot.CreateElement("CONDITION");
			XmlCondition.SetAttribute("TYPE", m_strConditionType);
			XmlCondition.SetAttribute("ACTION", m_strConditionActionType);
			m_pElement.AppendChild(XmlCondition);

			XmlElement XmlAction = XmlRoot.CreateElement("ACTION");
			XmlAction.SetAttribute("SCRIPT", m_strActionScript);

			if (m_strParameter1 != null)
				XmlAction.SetAttribute("PARAM", m_strParameter1);
			if (m_strParameter2 != null)
				XmlAction.SetAttribute("PARAM2", m_strParameter2);
			m_pElement.AppendChild(XmlAction);

			//m_pElement.DocumentElement.AppendChild(XmlTriggerElement);
		}

		public void SetNode(DirectoryInfo diInfo, XmlDocument XmlDocu, ref XmlElement pNode)
		{
			Initialize(diInfo);
			m_XmlDocument = XmlDocu;
			m_pElement = pNode;
		}

		// 기본 다이얼로그 폼에 정보를 채워 넣는다.
		public void SetData(string strTriggerID, string strConditionType, string strConditionActionType,
			string strActionScript, string strParameter1, string strParameter2,
			DirectoryInfo diInfo, XmlDocument XmlDocu, ref XmlElement pElement)
		{
			Initialize(diInfo);

			m_strTriggerID = strTriggerID;
			m_strConditionType = strConditionType;
			m_iConditionTypeIdx = FindConditionTypeIdx(m_strConditionType);
			m_strConditionActionType = strConditionActionType;
			m_iConditionActionTypeIdx = FindConditionActionTypeIdx(diInfo, strConditionActionType);
			m_strActionScript = strActionScript;
			m_strParameter1 = strParameter1;
			m_strParameter2 = strParameter2;

			// Xml
			m_pElement = pElement;
			m_XmlDocument = XmlDocu;

			m_tbTriggerID.AppendText(m_strTriggerID);
			m_cbConditionType.SelectedIndex = m_iConditionTypeIdx;
			m_cbActionType.SelectedIndex = m_iConditionActionTypeIdx;
			if (m_iConditionActionTypeIdx == -1)
				m_cbActionType.Text = strConditionActionType;
			m_tbActionScript.AppendText(m_strActionScript);
			if (m_strParameter1 != null)
				m_tbParameter1.AppendText(m_strParameter1);
			if (m_strParameter2 != null)
				m_tbParameter2.AppendText(m_strParameter2);
		}

		// string을 검색하여 ConditionType 의 인덱스를 얻는다.
		public int FindConditionTypeIdx(string strConditionType)
		{
			for (int i = 0; i < m_iConditionTypeCount; i++)
				if (m_strConditionTypeList[i].Equals(strConditionType))
					return i;
			return 0;
		}

		// string을 검색하여 ConditionActionType 의 인덱스를 얻는다.
		public int FindConditionActionTypeIdx(DirectoryInfo diInfo, string strActionType)
		{
			int i=0;
			string strDestFileName = strActionType + ".xml";

			if (diInfo.Exists)
			{
				foreach (FileInfo fiInfo in diInfo.GetFiles())
				{
					string strFileName = "a_" + fiInfo.Name;
					if (strFileName.Equals(strDestFileName))
						return i;
					i += 1;
				}
			}
			return -1;
		}

		private void m_btnOK_Click(object sender, EventArgs e)
		{
			SaveNode();
			this.Close();
		}

		private void m_btnCancel_Click(object sender, EventArgs e)
		{
			this.Close();
		}
	}
}
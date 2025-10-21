using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Xml;
using System.Collections;

namespace Emergent.Gamebryo.SceneDesigner.GUI.ProjectG
{
	public partial class NpcOptionForm : Form
	{
		public class NpcSay : object
		{
			public string m_strTYPE = "IDLE";
			public string m_strTTW = "83";
			public string m_strUPTIME = "6.3";
			public string m_strDELAY = "1.0";
			public string m_strACTION = "a_idle";
		}
		
		private string m_strLocX;
		private string m_strLocY;
		private string m_strLocZ;
		private XmlDocument m_XmlDocument;
		private XmlNode m_XmlNode;
		private XmlElement m_pElement;
		private ArrayList m_pmNpcSay = new ArrayList();  // StoneList의 리스트

		public NpcOptionForm()
		{
			InitializeComponent();

			m_cbType.SelectedIndex = 0;
            m_cbHidden.SelectedIndex = 0;
		}

		/*
		// 기본 다이얼로그 폼에 정보를 채워 넣는다.
		public void SetData(string strName, string strActor, string strScript,
			string strLocation, string strGuid, string strKid, string strType,
			XmlDocument XmlDocu, ref XmlElement pElement)
		{
			// Xml
			m_pElement = pElement;
			m_XmlDocument = XmlDocu;

			m_tbName.AppendText(strName);
			m_tbActor.AppendText(strActor);
			m_tbScript.AppendText(strScript);
			XML_LOCATION kLoc = new XML_LOCATION();
			kLoc = kLoc.StringsToLocation(strLocation);
			m_tbLocX.AppendText(Convert.ToString(kLoc.fLocX));
			m_tbLocY.AppendText(Convert.ToString(kLoc.fLocY));
			m_tbLocZ.AppendText(Convert.ToString(kLoc.fLocZ));
			m_tbGuid.AppendText(strGuid);
			m_tbKid.AppendText(strKid);
			m_cbType.SelectedItem = strType;
		}*/
		public void SetData(XmlNode kNode, XmlDocument XmlDocu, ref XmlElement pElement)
		{
			// attributes
			string sNpcName = "";
			string sNpcActor = "";
			string sNpcScript = "";
			string sNpcLocation = "";
			string sNpcGuid = "";
			string sNpcKid = "";
			string sType = "";
            string sHidden = "";
			if (kNode.Attributes.GetNamedItem("NAME") != null)
				sNpcName = kNode.Attributes.GetNamedItem("NAME").InnerText;
			if (kNode.Attributes.GetNamedItem("ACTOR") != null)
				sNpcActor = kNode.Attributes.GetNamedItem("ACTOR").InnerText;
			if (kNode.Attributes.GetNamedItem("SCRIPT") != null)
				sNpcScript = kNode.Attributes.GetNamedItem("SCRIPT").InnerText;
			if (kNode.Attributes.GetNamedItem("LOCATION") != null)
				sNpcLocation = kNode.Attributes.GetNamedItem("LOCATION").InnerText;
			if (kNode.Attributes.GetNamedItem("GUID") != null)
				sNpcGuid = kNode.Attributes.GetNamedItem("GUID").InnerText;
			if (kNode.Attributes.GetNamedItem("KID") != null)
				sNpcKid = kNode.Attributes.GetNamedItem("KID").InnerText;
			if (kNode.Attributes.GetNamedItem("TYPE") != null)
				sType = kNode.Attributes.GetNamedItem("TYPE").InnerText;
            if (kNode.Attributes.GetNamedItem("HIDDEN") != null)
                sHidden = kNode.Attributes.GetNamedItem("HIDDEN").InnerText;

			// Npc Say
			XmlNodeList nodeList = kNode.ChildNodes;
			foreach (XmlNode node in nodeList)
			{
				string strAdd = null;

				for (int i = 0; i < node.Attributes.Count; i++)
				{
					XmlNode childnode = node.Attributes.Item(i);
					strAdd = strAdd + " [" + childnode.InnerText + "]";
				}
				m_lbNpcSay.Items.Add(strAdd);

				NpcSay pNpcSay = new NpcSay();
				XmlNode pSayNode = null;
				if ((pSayNode = node.Attributes.GetNamedItem("TYPE")) != null)
				{
					pNpcSay.m_strTYPE = pSayNode.InnerText;
				}
				if ((pSayNode = node.Attributes.GetNamedItem("TTW")) != null)
				{
					pNpcSay.m_strTTW = pSayNode.InnerText;
				}
				if ((pSayNode = node.Attributes.GetNamedItem("UPTIME")) != null)
				{
					pNpcSay.m_strUPTIME = pSayNode.InnerText;
				}
				if ((pSayNode = node.Attributes.GetNamedItem("DELAY")) != null)
				{
					pNpcSay.m_strDELAY = pSayNode.InnerText;
				}
				if ((pSayNode = node.Attributes.GetNamedItem("ACTION")) != null)
				{
					pNpcSay.m_strACTION = pSayNode.InnerText;
				}
				m_pmNpcSay.Add(pNpcSay);
			}

			// Xml
			m_XmlNode = kNode;
			m_pElement = pElement;
			m_XmlDocument = XmlDocu;

			// Attribute
			m_tbName.AppendText(sNpcName);
			m_tbActor.AppendText(sNpcActor);
			m_tbScript.AppendText(sNpcScript);
			XML_LOCATION kLoc = new XML_LOCATION();
			kLoc = kLoc.StringsToLocation(sNpcLocation);
			m_tbLocX.AppendText(Convert.ToString(kLoc.fLocX));
			m_tbLocY.AppendText(Convert.ToString(kLoc.fLocY));
			m_tbLocZ.AppendText(Convert.ToString(kLoc.fLocZ));
			m_tbGuid.AppendText(sNpcGuid);
			m_tbKid.AppendText(sNpcKid);
			m_cbType.SelectedItem = sType;
            m_cbHidden.SelectedItem = sHidden;

			// Element in npc
			//XmlNodeList nodeList = kNode.getele
		}

		public void SetData(XmlDocument XmlDocu, ref XmlElement pElement)
		{
			// Xml
			m_pElement = pElement;
			m_XmlDocument = XmlDocu;
		}

		public void SaveData()
		{
			XmlDocument XmlRoot = m_XmlDocument;
			m_pElement.RemoveAll();

			if (m_tbName.Text != "")
				m_pElement.SetAttribute("NAME", m_tbName.Text);
			else
				m_pElement.SetAttribute("NAME", null);
			if (m_tbActor.Text != "")
				m_pElement.SetAttribute("ACTOR", m_tbActor.Text);
			else
				m_pElement.SetAttribute("ACTOR", null);
			if (m_tbScript.Text != "")
				m_pElement.SetAttribute("SCRIPT", m_tbScript.Text);
			else
				m_pElement.SetAttribute("SCRIPT", null);
			if (m_tbGuid.Text != "")
				m_pElement.SetAttribute("GUID", m_tbGuid.Text);
			else
				m_pElement.SetAttribute("GUID", null);
			if (m_tbKid.Text != "")
				m_pElement.SetAttribute("KID", m_tbKid.Text);
			else
				m_pElement.SetAttribute("KID", null);
			if (m_cbType.SelectedIndex != -1)
				m_pElement.SetAttribute("TYPE", m_cbType.SelectedItem.ToString());
			else
				m_pElement.SetAttribute("TYPE", null);

            if (m_cbHidden.SelectedIndex != -1)
                m_pElement.SetAttribute("HIDDEN", m_cbHidden.SelectedItem.ToString());
            else
                m_pElement.SetAttribute("HIDDEN", null);

			if (m_tbLocX.Text != "")
				m_strLocX = Convert.ToString(float.Parse(m_tbLocX.Text));
			else
				m_strLocX = "0";
			if (m_tbLocY.Text != "")
				m_strLocY = Convert.ToString(float.Parse(m_tbLocY.Text));
			else
				m_strLocY = "0";
			if (m_tbLocZ.Text != "")
				m_strLocZ = Convert.ToString(float.Parse(m_tbLocZ.Text));
			else
				m_strLocZ = "0";

			m_pElement.SetAttribute("LOCATION",
				m_strLocX + ", " +
				m_strLocY + ", " +
				m_strLocZ);
			m_pElement.SetAttribute("DIRECTION", "0, 0, 0");

			for (int i = 0; i < m_pmNpcSay.Count; i++)
			{
				NpcSay kSay = (NpcSay)m_pmNpcSay[i];
				XmlElement nodeSay = CreateSayNode(kSay.m_strTYPE, kSay.m_strTTW, kSay.m_strUPTIME, kSay.m_strDELAY, kSay.m_strACTION);

				m_pElement.AppendChild(nodeSay);
			}
		}

		private XmlElement CreateSayNode(string strTYPE, string strTTW, string strUPTIME, string strDELAY, string strACTION)
		{
			XmlElement nodeSay = m_XmlDocument.CreateElement("SAY");

			XmlAttribute attrSay = null;
			attrSay = m_XmlDocument.CreateAttribute("TYPE");
			attrSay.InnerText = strTYPE;
			nodeSay.Attributes.Append(attrSay);

			attrSay = m_XmlDocument.CreateAttribute("TTW");
			attrSay.InnerText = strTTW;
			nodeSay.Attributes.Append(attrSay);

			attrSay = m_XmlDocument.CreateAttribute("UPTIME");
			attrSay.InnerText = strUPTIME;
			nodeSay.Attributes.Append(attrSay);

			attrSay = m_XmlDocument.CreateAttribute("DELAY");
			attrSay.InnerText = strDELAY;
			nodeSay.Attributes.Append(attrSay);

			attrSay = m_XmlDocument.CreateAttribute("ACTION");
			attrSay.InnerText = strACTION;
			nodeSay.Attributes.Append(attrSay);

			return nodeSay;
		}

		private void m_btnOK_Click(object sender, EventArgs e)
		{
			SaveData();
			this.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.Close();
		}

		private void m_btnCancel_Click(object sender, EventArgs e)
		{
			this.Close();
		}

		private void m_btnGenerate_Click(object sender, EventArgs e)
		{
			m_tbGuid.Clear();
			m_tbGuid.AppendText(Guid.NewGuid().ToString());
		}

		private void m_btnSayAdd_Click(object sender, EventArgs e)
		{
			int iSelect = m_lbNpcSay.SelectedIndex;

			NpcSayForm frm = new NpcSayForm();
			if (DialogResult.OK == frm.ShowDialog())
			{
				NpcSay pNpcSay = new NpcSay();
				pNpcSay.m_strTYPE = frm.m_strTYPE;
				pNpcSay.m_strTTW = frm.m_strTTW;
				pNpcSay.m_strUPTIME = frm.m_strUPTIME;
				pNpcSay.m_strDELAY = frm.m_strDELAY;
				pNpcSay.m_strACTION = frm.m_strACTION;

				string strAdd = "";
				XmlElement nodeSay = CreateSayNode(frm.m_strTYPE, frm.m_strTTW, frm.m_strUPTIME, frm.m_strDELAY, frm.m_strACTION);
				for (int i = 0; i < nodeSay.Attributes.Count; i++)
				{
					XmlNode childnode = nodeSay.Attributes.Item(i);
					strAdd = strAdd + " [" + childnode.InnerText + "]";
				}

				if (iSelect == -1)
				{
					m_pmNpcSay.Add(pNpcSay);
					m_lbNpcSay.Items.Add(strAdd);
				}
				else
				{
					m_pmNpcSay.Insert(iSelect, pNpcSay);
					m_lbNpcSay.Items.Insert(iSelect, strAdd);
				}
			}
		}

		private void m_btnSayDel_Click(object sender, EventArgs e)
		{
			int iSelect = m_lbNpcSay.SelectedIndex;
			if (iSelect == -1)
				return;

			DialogResult result = MessageBox.Show(
				"선택한 SAY를 지웁니다.",
				"경고",
				MessageBoxButtons.OKCancel);
			if (result != DialogResult.OK)
			{
				return;
			}

			m_pmNpcSay.RemoveAt(iSelect);
			m_lbNpcSay.Items.RemoveAt(iSelect);
		}

		private void ModifySay()
		{
			int iSelect = m_lbNpcSay.SelectedIndex;
			if (iSelect == -1)
				return;

			NpcSay kSay = (NpcSay)m_pmNpcSay[iSelect];

			NpcSayForm frm = new NpcSayForm();
			frm.SetData(kSay.m_strTYPE, kSay.m_strTTW, kSay.m_strUPTIME, kSay.m_strDELAY, kSay.m_strACTION);

			if (DialogResult.OK == frm.ShowDialog())
			{
				m_pmNpcSay.RemoveAt(iSelect);
				m_lbNpcSay.Items.RemoveAt(iSelect);

				// Insert
				NpcSay pNpcSay = new NpcSay();
				pNpcSay.m_strTYPE = frm.m_strTYPE;
				pNpcSay.m_strTTW = frm.m_strTTW;
				pNpcSay.m_strUPTIME = frm.m_strUPTIME;
				pNpcSay.m_strDELAY = frm.m_strDELAY;
				pNpcSay.m_strACTION = frm.m_strACTION;
				m_pmNpcSay.Insert(iSelect, pNpcSay);

				string strAdd = "";
				XmlElement nodeSay = CreateSayNode(frm.m_strTYPE, frm.m_strTTW, frm.m_strUPTIME, frm.m_strDELAY, frm.m_strACTION);
				for (int i = 0; i < nodeSay.Attributes.Count; i++)
				{
					XmlNode childnode = nodeSay.Attributes.Item(i);
					strAdd = strAdd + " [" + childnode.InnerText + "]";
				}
				m_lbNpcSay.Items.Insert(iSelect, strAdd);
				m_lbNpcSay.SelectedIndex = iSelect;
			}
		}

		private void m_btnSayModify_Click(object sender, EventArgs e)
		{
			ModifySay();
		}

		private void m_btnSayClear_Click(object sender, EventArgs e)
		{
			DialogResult result = MessageBox.Show(
				"모든 SAY를 지웁니다.",
				"경고",
				MessageBoxButtons.OKCancel);
			if (result != DialogResult.OK)
			{
				return;
			}

			m_pmNpcSay.Clear();
			m_lbNpcSay.Items.Clear();
		}

		private void SwapData(int iIdx, int iIdx2)
		{
			if (iIdx == iIdx2)
			{
				return;
			}

			Object kPMSay = m_pmNpcSay[iIdx];
			Object kLBSay = m_lbNpcSay.Items[iIdx];
			Object kPMSay2 = m_pmNpcSay[iIdx2];
			Object kLBSay2 = m_lbNpcSay.Items[iIdx2];

			if (iIdx < iIdx2)
			{
				m_pmNpcSay.RemoveAt(iIdx2);
				m_lbNpcSay.Items.RemoveAt(iIdx2);
				m_pmNpcSay.RemoveAt(iIdx);
				m_lbNpcSay.Items.RemoveAt(iIdx);

				m_pmNpcSay.Insert(iIdx, kPMSay2);
				m_lbNpcSay.Items.Insert(iIdx, kLBSay2);
				m_pmNpcSay.Insert(iIdx2, kPMSay);
				m_lbNpcSay.Items.Insert(iIdx2, kLBSay);
			}
			else
			{
				m_pmNpcSay.RemoveAt(iIdx);
				m_lbNpcSay.Items.RemoveAt(iIdx);
				m_pmNpcSay.RemoveAt(iIdx2);
				m_lbNpcSay.Items.RemoveAt(iIdx2);

				m_pmNpcSay.Insert(iIdx2, kPMSay);
				m_lbNpcSay.Items.Insert(iIdx2, kLBSay);
				m_pmNpcSay.Insert(iIdx, kPMSay2);
				m_lbNpcSay.Items.Insert(iIdx, kLBSay2);
			}
		}

		private void m_btnSayMoveUp_Click(object sender, EventArgs e)
		{
			int iSelect = m_lbNpcSay.SelectedIndex;
			if (iSelect == 0 || iSelect == -1)
			{
				return;
			}

			SwapData(iSelect, iSelect - 1);
			m_lbNpcSay.SelectedIndex = iSelect - 1;
		}

		private void m_btnSayMoveDown_Click(object sender, EventArgs e)
		{
			int iSelect = m_lbNpcSay.SelectedIndex;
			if (iSelect == m_lbNpcSay.Items.Count - 1 || iSelect == -1)
			{
				return;
			}

			SwapData(iSelect, iSelect + 1);
			m_lbNpcSay.SelectedIndex = iSelect + 1;
		}

		private void m_lbNpcSay_MouseDoubleClick(object sender, MouseEventArgs e)
		{
			int iSelect = m_lbNpcSay.SelectedIndex;
			if (iSelect == -1)
				return;

			ModifySay();
			m_lbNpcSay.SelectedIndex = iSelect;
		}
	}
}
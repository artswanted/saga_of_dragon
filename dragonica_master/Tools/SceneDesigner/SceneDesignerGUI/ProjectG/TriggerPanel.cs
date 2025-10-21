using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using System.Collections;
using System.Threading;
using System.Xml;
using System.IO;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;
using Emergent.Gamebryo.SceneDesigner.GUI.ProjectG;

namespace Emergent.Gamebryo.SceneDesigner.GUI
{
	public partial class TriggerPanel : Form
	{
		#region Private Data
		//private ISelectionSetService m_selectionSetService;
		//private ISelectionService ms_pmSelectionService = null;
		private IUICommandService m_uiCommandService;
		//private ServiceProvider m_serviceProvider;
		// Todo : Xml 관리자
		private XmlManager m_XmlManager;
		private XmlDocument m_XmlDocument;
		private XmlElement m_pElement;
		#endregion

		public TriggerPanel()
		{
			InitializeComponent();

			UICommandService.BindCommands(this);
		}

		private static IInteractionModeService m_pmInteractionModeService;
		private static IInteractionModeService InteractionModeService
		{
			get
			{
				if (m_pmInteractionModeService == null)
				{
					m_pmInteractionModeService = ServiceProvider.Instance
						.GetService(typeof(IInteractionModeService)) as
						IInteractionModeService;
					Debug.Assert(m_pmInteractionModeService != null,
						"Interaction mode service not found!");
				}
				return m_pmInteractionModeService;
			}
		}

		//private ISelectionSetService SelectionSetService
		//{
		//	get
		//	{
		//		if (m_selectionSetService == null)
		//		{

		//			m_selectionSetService =
		//				ServiceProvider.Instance.GetService(
		//				typeof(ISelectionSetService)) as ISelectionSetService;
		//		}
		//		return m_selectionSetService;
		//	}
		//}

		private IUICommandService UICommandService
		{
			get
			{
				if (m_uiCommandService == null)
				{
					m_uiCommandService =
						ServiceProvider.Instance.GetService(
						typeof(IUICommandService))
						as IUICommandService;
				}
				return m_uiCommandService;
			}
		}

		// PanelListLoad()
		// Description : XmlOpen 을 통해서 불러온 Xml을 해부하여 List한다.
		public void PanelListLoad()
		{
			m_lbTriggerSets.Items.Clear();

			XmlDocument pDoc = m_XmlManager.GetDocument();
			XmlNodeList nodeList = pDoc.GetElementsByTagName("TRIGGER");

			// Trigger 수만큼 루프
			foreach (XmlNode node in nodeList)
			{
				string strAdd;
				strAdd = node.Attributes.GetNamedItem("ID").InnerText + ":";

				// Trigger 안 ChildNode 수 만큼 루프
				for (int i=0 ; i<node.ChildNodes.Count ; i++)
				{
					XmlNode childnode = node.ChildNodes.Item(i);
					string strChildType = childnode.GetType().Name;

					if (strChildType.Equals("XmlComment"))
						continue;

					// 이 엘리먼트들은 지정되어져 있다.
					if (childnode.Name.Equals("CONDITION"))
					{
						for (int j = 0; j < childnode.Attributes.Count; j++)
						{
							if (childnode.Attributes.Item(j).Name.Equals("TYPE") ||
								childnode.Attributes.Item(j).Name.Equals("ACTION"))
							{
								strAdd = strAdd + " [" +
									childnode.Attributes.Item(j).InnerText + "]";
							}
							else
							{
								//MessageBox.Show("지정되지 않은 형식이 들어가 있습니다.");
								//m_lbTriggerSets.Items.Clear();
								//return;
							}
						}
					}
					else if (childnode.Name.Equals("ACTION"))
					{
						for (int j = 0; j < childnode.Attributes.Count; j++)
						{
							if (childnode.Attributes.Item(j).Name.Equals("SCRIPT") ||
								childnode.Attributes.Item(j).Name.Equals("PARAM") ||
								childnode.Attributes.Item(j).Name.Equals("PARAM2"))
							{
								strAdd = strAdd + " [" +
									childnode.Attributes.Item(j).InnerText + "]";
							}
							else
							{
								//MessageBox.Show("지정되지 않은 형식이 들어가 있습니다.");
								//m_lbTriggerSets.Items.Clear();
								//return;
							}
						}
					}
					else
					{
						// Error : 지정되지 않은 형식이 들어가 있습니다.
						//MessageBox.Show("지정되지 않은 형식이 들어가 있습니다.");
						//m_lbTriggerSets.Items.Clear();
						//return;
					}
				}

				m_lbTriggerSets.Items.Add(strAdd);
			}
		}

		public void SetXmlManager(ref XmlManager pXmlManager)
		{
			m_XmlManager = pXmlManager;
		}

		public void ModifyTrigger()
		{
			TriggerOptionForm frm = new TriggerOptionForm();
			int iIndex = m_lbTriggerSets.SelectedIndex;
			if (iIndex == -1) return;
			XmlDocument pDoc = m_XmlManager.GetDocument();
			if (pDoc == null) return;
			XmlNodeList nodeList = pDoc.GetElementsByTagName("TRIGGER");
			XmlNode node = nodeList.Item(iIndex);

			string sTriggerID = node.Attributes.GetNamedItem("ID").InnerText;
			string sConditionType = null;
			string sConditionAction = null;
			string sActionScript = null;
			string sActionParameter = null;
			string sActionParameter2 = null;

			for (int i = 0; i < node.ChildNodes.Count; i++)
			{
				XmlNode childnode = node.ChildNodes.Item(i);

				if (childnode.Name.Equals("CONDITION"))
				{
					for (int j = 0; j < childnode.Attributes.Count; j++)
					{
						if (childnode.Attributes.Item(j).Name.Equals("TYPE"))
							sConditionType = childnode.Attributes.Item(j).InnerText;
						else if (childnode.Attributes.Item(j).Name.Equals("ACTION"))
							sConditionAction = childnode.Attributes.Item(j).InnerText;
					}
				}
				else if (childnode.Name.Equals("ACTION"))
				{
					for (int j = 0; j < childnode.Attributes.Count; j++)
					{
						if (childnode.Attributes.Item(j).Name.Equals("SCRIPT"))
							sActionScript = childnode.Attributes.Item(j).InnerText;
						else if (childnode.Attributes.Item(j).Name.Equals("PARAM"))
							sActionParameter = childnode.Attributes.Item(j).InnerText;
						else if (childnode.Attributes.Item(j).Name.Equals("PARAM2"))
							sActionParameter2 = childnode.Attributes.Item(j).InnerText;
					}
				}
			}

			//m_pNode = m_XmlManager.GetDocument();
			m_XmlDocument = m_XmlManager.GetDocument();
			m_pElement = m_XmlDocument.CreateElement("TRIGGER");
			frm.SetData(sTriggerID, sConditionType, sConditionAction, sActionScript,
				sActionParameter, sActionParameter2, m_XmlManager.GetDirectoryInfo(), m_XmlDocument, ref m_pElement);
			if (DialogResult.OK == frm.ShowDialog())
			{
				XmlElement pRoot = m_XmlManager.GetDocument().DocumentElement;
				pRoot.ReplaceChild(m_pElement, node);
			}

			PanelListLoad();
			m_lbTriggerSets.SelectedIndex = iIndex;
		}

		private void SwapNode(XmlNodeList nodeList, int inode1Idx, XmlNode node1, XmlNode node2)
		{
			XmlElement pRoot = m_XmlManager.GetDocument().DocumentElement;
			pRoot.InsertAfter(node1, node2);
			pRoot.InsertBefore(node2, nodeList.Item(inode1Idx));
		}

		#region WinForms event handlers

		[UICommandHandler("Idle")]
		private void Application_Idle(object sender, EventArgs e)
		{
			// 항시 윈도우 창 고정.
			if (m_lbTriggerSets.Width != this.Width - 25 ||
				m_lbTriggerSets.Height != this.Height - 100)
			{
				m_lbTriggerSets.Width = this.Width - 25;
				m_lbTriggerSets.Height = this.Height - 100;

				// button
				Point ptLoc = new Point();
				ptLoc.X = m_lbTriggerSets.Location.X + (int)(this.Width * 0.5f) - 110;
				ptLoc.Y = m_lbTriggerSets.Location.Y + m_lbTriggerSets.Height + 8;
				m_btnAdd.Location = ptLoc;

				ptLoc.X = ptLoc.X + m_btnAdd.Width + 8;
				m_btnDelete.Location = ptLoc;

				ptLoc.X = ptLoc.X + m_btnDelete.Width + 8;
				m_btnCopy.Location = ptLoc;

				//
				ptLoc.X = m_lbTriggerSets.Location.X + (int)(this.Width * 0.5f) - 110; ;
				ptLoc.Y = ptLoc.Y + m_btnCopy.Height + 8;
				m_btnModify.Location = ptLoc;

				ptLoc.X = ptLoc.X + m_btnModify.Width + 8;
				m_btnMoveUp.Location = ptLoc;

				ptLoc.X = ptLoc.X + m_btnMoveUp.Width + 8;
				m_btnMoveDown.Location = ptLoc;

				ptLoc.X = ptLoc.X + m_btnMoveDown.Width + 8;
				m_btnClear.Location = ptLoc;
			}
		}

		private void m_btnAdd_Click(object sender, EventArgs e)
		{
			XmlDocument pDoc = m_XmlManager.GetDocument();
			if (pDoc == null) return;
			int iIndex = m_lbTriggerSets.SelectedIndex;
			XmlNodeList nodeList = m_XmlManager.GetDocument().GetElementsByTagName("TRIGGER");
			XmlNode node = null;

			bool bIsNothing=false;
			if (iIndex != -1)
				node = nodeList.Item(iIndex);
			else if (nodeList.Count == 0)
				bIsNothing = true;
			else
				bIsNothing = true;

			TriggerOptionForm frm = new TriggerOptionForm();
			m_XmlDocument = m_XmlManager.GetDocument();
			m_pElement = m_XmlDocument.CreateElement("TRIGGER");
			frm.SetNode(m_XmlManager.GetDirectoryInfo(), m_XmlDocument, ref m_pElement);
			if (DialogResult.OK == frm.ShowDialog())
			{
				XmlElement pRoot = m_XmlManager.GetDocument().DocumentElement;

				if (bIsNothing==false)
					pRoot.InsertAfter(m_pElement, node);
				else
					pRoot.AppendChild(m_pElement);

				PanelListLoad();
				if (bIsNothing == false)
					m_lbTriggerSets.SelectedIndex = iIndex + 1;
				else
					m_lbTriggerSets.SelectedIndex = 0;
			}
		}

		private void m_lbTriggerSets_DoubleClick(object sender, EventArgs e)
		{
			TriggerOptionForm frm = new TriggerOptionForm();
			int iIndex = m_lbTriggerSets.SelectedIndex;
			if (iIndex == -1) return;
			XmlDocument pDoc = m_XmlManager.GetDocument();
			if (pDoc == null) return;
			XmlNodeList nodeList = pDoc.GetElementsByTagName("TRIGGER");
			XmlNode node = nodeList.Item(iIndex);

			string sNpcName = node.Attributes.GetNamedItem("ID").InnerText;
			MEntity[] pkEntities = new MEntity[1];
			pkEntities[0] = MFramework.Instance.Scene.GetEntityByName(sNpcName);
			if (pkEntities[0] == null)
				return;

			ISelectionService pmSelectionService =
				ServiceProvider.Instance.GetService(
				typeof(ISelectionService)) as ISelectionService;
			pmSelectionService.ClearSelectedEntities();
			pmSelectionService.AddEntitiesToSelection(pkEntities);

			InteractionModeService.GetInteractionModeByName("GamebryoView").MouseMove(-999999, -999999);
		}

		private void m_btnModify_Click(object sender, EventArgs e)
		{
			ModifyTrigger();
		}

		private void m_btnDelete_Click(object sender, EventArgs e)
		{
			int iIndex = m_lbTriggerSets.SelectedIndex;
			if (iIndex == -1) return;
			XmlDocument pDoc = m_XmlManager.GetDocument();
			if (pDoc == null) return;
			XmlElement pRoot = m_XmlManager.GetDocument().DocumentElement;

			XmlNodeList nodeList = pDoc.GetElementsByTagName("TRIGGER");
			XmlNode node = nodeList.Item(iIndex);
			pRoot.RemoveChild(node);
			PanelListLoad();

			if (iIndex >= nodeList.Count)
				m_lbTriggerSets.SelectedIndex = nodeList.Count - 1;
			else
				m_lbTriggerSets.SelectedIndex = iIndex;
		}

		private void m_btnCopy_Click(object sender, EventArgs e)
		{
			int iIndex = m_lbTriggerSets.SelectedIndex;
			if (iIndex == -1) return;
			XmlDocument pDoc = m_XmlManager.GetDocument();
			if (pDoc == null) return;
			XmlElement pRoot = m_XmlManager.GetDocument().DocumentElement;

			XmlNodeList nodeList = pDoc.GetElementsByTagName("TRIGGER");
			XmlNode node = nodeList.Item(iIndex);

			// Todo
			XmlNode nodeClone = node.Clone();
			pRoot.InsertAfter(nodeClone, node);

			PanelListLoad();
			m_lbTriggerSets.SelectedIndex = iIndex + 1;
		}

		private void m_btnClear_Click(object sender, EventArgs e)
		{
			Question frm = new Question();
			XmlElement pRoot = m_XmlManager.GetDocument().DocumentElement;
			if (DialogResult.OK == frm.ShowDialog())
			{
				XmlDocument pDoc = m_XmlManager.GetDocument();
				XmlNodeList nodeList = pDoc.GetElementsByTagName("TRIGGER");
				int iCount = nodeList.Count;

				for (int i = 0; i < iCount; i++)
				{
					XmlNode node = nodeList.Item(0);
					pRoot.RemoveChild(node);
				}
			}
			PanelListLoad();
		}

		private void m_btnMoveUp_Click(object sender, EventArgs e)
		{
			int iIndex = m_lbTriggerSets.SelectedIndex;
			if (iIndex == -1) return;
			XmlDocument pDoc = m_XmlManager.GetDocument();
			XmlNodeList nodeList = pDoc.GetElementsByTagName("TRIGGER");

			if (iIndex <= 0 ||
				iIndex >= nodeList.Count)
				return;
			if (pDoc == null) return;
			XmlElement pRoot = m_XmlManager.GetDocument().DocumentElement;

			XmlNode node = nodeList.Item(iIndex-1);
			XmlNode node2 = nodeList.Item(iIndex);

			SwapNode(nodeList, iIndex, node, node2);

			PanelListLoad();
			m_lbTriggerSets.SelectedIndex = iIndex - 1;
		}

		private void m_btnMoveDown_Click(object sender, EventArgs e)
		{
			int iIndex = m_lbTriggerSets.SelectedIndex;
			if (iIndex == -1) return;
			XmlDocument pDoc = m_XmlManager.GetDocument();
			XmlNodeList nodeList = pDoc.GetElementsByTagName("TRIGGER");

			if (iIndex < 0 ||
				iIndex >= nodeList.Count-1)
				return;
			if (pDoc == null) return;
			XmlElement pRoot = m_XmlManager.GetDocument().DocumentElement;

			XmlNode node = nodeList.Item(iIndex);
			XmlNode node2 = nodeList.Item(iIndex+1);

			SwapNode(nodeList, iIndex, node, node2);

			PanelListLoad();
			m_lbTriggerSets.SelectedIndex = iIndex + 1;
		}

		#endregion

		private void m_lbTriggerSets_MouseDown(object sender, MouseEventArgs e)
		{
			if (e.Button.ToString().Equals("Right"))
			{
				ModifyTrigger();
			}
		}
	}
}
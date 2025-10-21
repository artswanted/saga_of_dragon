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
	public partial class NpcPanel : Form
	{
		#region Private Data
		private IUICommandService m_uiCommandService;
		// Xml 관리자
		private XmlManager m_XmlManager;
		private XmlDocument m_XmlDocument;
		private XmlElement m_pElement;

		#endregion

		public NpcPanel()
		{
			InitializeComponent();

			UICommandService.BindCommands(this);
		}

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

		public void InitPanelListLoad()
		{
			m_lbNpcSets.Items.Clear();

			XmlDocument pDoc = m_XmlManager.GetDocument();
			XmlNodeList nodeList = pDoc.GetElementsByTagName("NPC");

			MPalette pkPalette = MFramework.Instance.PaletteManager.GetPaletteByName("General");
			if (pkPalette == null)
			{
				MessageBox.Show("General 팔레트가 없습니다.");
				return;
			}
			MEntity pkTemplate = pkPalette.GetEntityByName("[General]Target.npc_target");
			if (pkTemplate == null)
			{
				MessageBox.Show("[General]Target.npc_target 가 없습니다.");
				return;
			}

			// Npc 수만큼 루프
			foreach (XmlNode node in nodeList)
			{
				string strAdd = null;

				for (int i = 0; i < node.Attributes.Count; i++)
				{
					XmlNode childnode = node.Attributes.Item(i);

					strAdd = strAdd + " [" +
						childnode.InnerText + "]";
				}

				m_lbNpcSets.Items.Add(strAdd);

				// Create NPC Entity
				MEntity pkNewEntity;
				pkNewEntity = pkTemplate.Clone(node.Attributes.GetNamedItem("NAME").InnerText, true);
				AddNpc(
					node.Attributes.GetNamedItem("LOCATION") != null ? node.Attributes.GetNamedItem("LOCATION").InnerText : null,
					node.Attributes.GetNamedItem("DIRECTION") != null ? node.Attributes.GetNamedItem("DIRECTION").InnerText : null,
					pkNewEntity);
			}
		}

		public void AddNpc(string strLocation, string strDirection, MEntity pkNewEntity)
		{
			XML_LOCATION kLoc = new XML_LOCATION();
			XML_LOCATION kDir = new XML_LOCATION();
			MPoint3 translation = new MPoint3();
			MPoint3 rotation = new MPoint3();

			if (strLocation != null)
			{
				kLoc = kLoc.StringsToLocation(strLocation);
				translation.X = kLoc.fLocX;
				translation.Y = kLoc.fLocY;
				translation.Z = kLoc.fLocZ;
			}
			if (strDirection != null)
			{
				kDir = kDir.StringsToLocation(strDirection);
				rotation.X = kDir.fLocX;
				rotation.Y = kDir.fLocY;
				rotation.Z = kDir.fLocZ;
			}

			MFramework.Instance.Npc.InsertNpc(translation, rotation, pkNewEntity);
		}

		// PanelListLoad()
		// Description : XmlOpen 을 통해서 불러온 Xml을 해부하여 List한다.
		public void PanelListLoad()
		{
			m_lbNpcSets.Items.Clear();

			XmlDocument pDoc = m_XmlManager.GetDocument();
			XmlNodeList nodeList = pDoc.GetElementsByTagName("NPC");

			MPalette pkPalette = MFramework.Instance.PaletteManager.GetPaletteByName("General");
			if (pkPalette == null)
			{
				MessageBox.Show("General 팔레트가 없습니다.");
				return;
			}
			MEntity pkTemplate = pkPalette.GetEntityByName("[General]Target.npc_target");
			if (pkTemplate == null)
			{
				MessageBox.Show("[General]Target.npc_target 가 없습니다.");
				return;
			}

			// Npc 수만큼 루프
			foreach (XmlNode node in nodeList)
			{
				string strAdd = null;

				for (int i = 0; i < node.Attributes.Count; i++)
				{
					XmlNode childnode = node.Attributes.Item(i);

					strAdd = strAdd + " [" +
						childnode.InnerText + "]";
				}

				m_lbNpcSets.Items.Add(strAdd);
			}
		}

		public void SetXmlManager(ref XmlManager pXmlManager)
		{
			m_XmlManager = pXmlManager;
		}

		public void ModifyNpc()
		{
			NpcOptionForm frm = new NpcOptionForm();
			int iIndex = m_lbNpcSets.SelectedIndex;
			if (iIndex == -1) return;
			XmlDocument pDoc = m_XmlManager.GetDocument();
			if (pDoc == null) return;
			XmlNodeList nodeList = pDoc.GetElementsByTagName("NPC");
			XmlNode node = nodeList.Item(iIndex);

			string sNpcName = "";
			string sNpcActor = "";
			string sNpcScript = "";
			string sNpcLocation = "";
			string sNpcGuid = "";
			string sNpcKid = "";
			string sType = "";
            string sHidden = "";
			
			if (node.Attributes.GetNamedItem("NAME") != null)
				sNpcName = node.Attributes.GetNamedItem("NAME").InnerText;
			if (node.Attributes.GetNamedItem("ACTOR") != null)
				sNpcActor = node.Attributes.GetNamedItem("ACTOR").InnerText;
			if (node.Attributes.GetNamedItem("SCRIPT") != null)
				sNpcScript = node.Attributes.GetNamedItem("SCRIPT").InnerText;
			if (node.Attributes.GetNamedItem("LOCATION") != null)
				sNpcLocation = node.Attributes.GetNamedItem("LOCATION").InnerText;
			if (node.Attributes.GetNamedItem("GUID") != null)
				sNpcGuid = node.Attributes.GetNamedItem("GUID").InnerText;
			if (node.Attributes.GetNamedItem("KID") != null)
				sNpcKid = node.Attributes.GetNamedItem("KID").InnerText;
			if (node.Attributes.GetNamedItem("TYPE") != null)
				sType = node.Attributes.GetNamedItem("TYPE").InnerText;
            if (node.Attributes.GetNamedItem("HIDDEN") != null)
                sHidden = node.Attributes.GetNamedItem("HIDDEN").InnerText;

			m_XmlDocument = m_XmlManager.GetDocument();
			m_pElement = m_XmlDocument.CreateElement("NPC");

			frm.SetData(node, m_XmlDocument, ref m_pElement);
			//frm.SetData(
			//	sNpcName, sNpcActor, sNpcScript, sNpcLocation,
			//	sNpcGuid, sNpcKid, sType,
			//	m_XmlDocument, ref m_pElement);
			if (DialogResult.OK == frm.ShowDialog())
			{
				// Entity Apply
				XML_LOCATION kLoc = new XML_LOCATION();
				kLoc = kLoc.StringsToLocation(m_pElement.GetAttribute("LOCATION"));
				MPoint3 translation = new MPoint3(kLoc.fLocX, kLoc.fLocY, kLoc.fLocZ);

				MEntity pkModifyEntity = MFramework.Instance.Scene.GetEntityByName(sNpcName);
				pkModifyEntity.SetPropertyData("Translation", translation, false);
				pkModifyEntity.Name = m_pElement.GetAttribute("NAME");
				m_pElement.SetAttribute("NAME", pkModifyEntity.Name);   // 이름이 안 바뀌었을때를 대비해서

				XmlElement pRoot = m_XmlManager.GetDocument().DocumentElement;
				pRoot.ReplaceChild(m_pElement, node);
			}

			PanelListLoad();
			m_lbNpcSets.SelectedIndex = iIndex;
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
			if (m_lbNpcSets.Width != this.Width - 25 ||
				m_lbNpcSets.Height != this.Height - 100)
			{
				m_lbNpcSets.Width = this.Width - 25;
				m_lbNpcSets.Height = this.Height - 100;

				// button
				Point ptLoc = new Point();
				ptLoc.X = m_lbNpcSets.Location.X + (int)(this.Width * 0.5f) - 110;
				ptLoc.Y = m_lbNpcSets.Location.Y + m_lbNpcSets.Height + 8;
				m_btnAdd.Location = ptLoc;

				ptLoc.X = ptLoc.X + m_btnAdd.Width + 8;
				m_btnDelete.Location = ptLoc;

				ptLoc.X = ptLoc.X + m_btnDelete.Width + 8;
				m_btnCopy.Location = ptLoc;

				//
				ptLoc.X = m_lbNpcSets.Location.X + (int)(this.Width * 0.5f) - 110; ;
				ptLoc.Y = ptLoc.Y + m_btnCopy.Height + 8;
				m_btnModify.Location = ptLoc;

				ptLoc.X = ptLoc.X + m_btnModify.Width + 8;
				m_btnMoveUp.Location = ptLoc;

				ptLoc.X = ptLoc.X + m_btnMoveUp.Width + 8;
				m_btnMoveDown.Location = ptLoc;

				ptLoc.X = ptLoc.X + m_btnMoveDown.Width + 8;
				m_btnClear.Location = ptLoc;
			}

			// if Npc Entity was deleted
			if (MFramework.Instance.Npc.IsDeletedNpcData)
			{
				XmlDocument pDoc = m_XmlManager.GetDocument();
				if (pDoc == null) return;
				XmlElement pRoot = m_XmlManager.GetDocument().DocumentElement;
				XmlNodeList nodeList = pDoc.GetElementsByTagName("NPC");
				int iCount = MFramework.Instance.Npc.CountDeletedNpcData();
				for (int i = 0; i < iCount; i++)
				{
					string strName = MFramework.Instance.Npc.PopDeletedNpcData();

					for (int j = 0; j < nodeList.Count; j++)
					{
						string strDest = nodeList.Item(j).Attributes.GetNamedItem("NAME").InnerText;
						if (strDest.Equals(strName))
						{
							XmlNode node = nodeList.Item(j);
							pRoot.RemoveChild(node);
							j -= 1;
						}
					}
				}

				PanelListLoad();
				MFramework.Instance.Npc.IsDeletedNpcData = false;
			}

			if (MFramework.Instance.Npc.IsModifiedNpcData)
			{
				m_lbNpcSets.Items.Clear();

				XmlDocument pDoc = m_XmlManager.GetDocument();
				XmlNodeList nodeList = pDoc.GetElementsByTagName("NPC");
				for (int i=0 ; i<nodeList.Count ; i++)
				{
					XmlNode node = nodeList.Item(i);
					string sNpcName = node.Attributes.GetNamedItem("NAME").InnerText;

					if (MFramework.Instance.Npc.GetModifiedNpcData_IsNamed(sNpcName))
					{
						node.Attributes.GetNamedItem("NAME").InnerText =
							MFramework.Instance.Npc.GetModifiedNpcData_DestName(sNpcName);
					}

					if (MFramework.Instance.Npc.GetModifiedNpcData_IsTranslate(sNpcName))
					{
						float fX = MFramework.Instance.Npc.GetModifiedNpcData_PosX(sNpcName);
						float fY = MFramework.Instance.Npc.GetModifiedNpcData_PosY(sNpcName);
						float fZ = MFramework.Instance.Npc.GetModifiedNpcData_PosZ(sNpcName);

						node.Attributes.GetNamedItem("LOCATION").InnerText =
							fX.ToString() + ", " +
							fY.ToString() + ", " +
							fZ.ToString();
					}

					if (MFramework.Instance.Npc.GetModifiedNpcData_IsRotate(sNpcName))
					{
						float fX = MFramework.Instance.Npc.GetModifiedNpcData_RotateX(sNpcName);
						float fY = MFramework.Instance.Npc.GetModifiedNpcData_RotateY(sNpcName);
						float fZ = MFramework.Instance.Npc.GetModifiedNpcData_RotateZ(sNpcName);

						node.Attributes.GetNamedItem("DIRECTION").InnerText =
							fX.ToString() + ", " +
							fY.ToString() + ", " +
							fZ.ToString();
					}
				}

				PanelListLoad();
				MFramework.Instance.Npc.ClearModifiedNpcData();
				MFramework.Instance.Npc.IsModifiedNpcData = false;
			}

			if (MFramework.Instance.Npc.IsSelectedData)
			{
				XmlDocument pDoc = m_XmlManager.GetDocument();
				if (pDoc == null) return;
				XmlElement pRoot = m_XmlManager.GetDocument().DocumentElement;
				XmlNodeList nodeList = pDoc.GetElementsByTagName("NPC");
				string strName = MFramework.Instance.Npc.m_pkSelectedEntity.Name;

				for (int j = 0; j < nodeList.Count; j++)
				{
					string strDest = nodeList.Item(j).Attributes.GetNamedItem("NAME").InnerText;
					if (strDest.Equals(strName))
					{
						m_lbNpcSets.SelectedIndex = j;
						break;
					}
				}

				//PanelListLoad();
				MFramework.Instance.Npc.IsSelectedData = false;
				MFramework.Instance.Npc.SetSelectEntity(null);
			}
		}

		private void m_btnAdd_Click(object sender, EventArgs e)
		{
			XmlDocument pDoc = m_XmlManager.GetDocument();
			if (pDoc == null) return;
			int iIndex = m_lbNpcSets.SelectedIndex;
			XmlNodeList nodeList = m_XmlManager.GetDocument().GetElementsByTagName("NPC");
			XmlNode node = null;

			bool bIsNothing = false;
			if (iIndex != -1)
				node = nodeList.Item(iIndex);
			else if (nodeList.Count == 0)
				bIsNothing = true;
			else
				bIsNothing = true;

			NpcOptionForm frm = new NpcOptionForm();
			m_XmlDocument = m_XmlManager.GetDocument();
			m_pElement = m_XmlDocument.CreateElement("NPC");
			frm.SetData(m_XmlDocument, ref m_pElement);
			if (DialogResult.OK == frm.ShowDialog())
			{
				// Create NPC Entity
				MPalette pkPalette = MFramework.Instance.PaletteManager.GetPaletteByName("General");
				if (pkPalette == null)
				{
					MessageBox.Show("General 팔레트가 없습니다.");
					return;
				}
				MEntity pkTemplate = pkPalette.GetEntityByName("[General]Target.npc_target");
				if (pkTemplate == null)
				{
					MessageBox.Show("[General]Target.npc_target 가 없습니다.");
					return;
				}
				
				MEntity pkNewEntity;
				pkNewEntity = pkTemplate.Clone(m_pElement.GetAttribute("NAME"), true);
				AddNpc(
					m_pElement.GetAttribute("LOCATION"),
					m_pElement.GetAttribute("DIRECTION"),
					pkNewEntity);

				// Insert Node
				XmlElement pRoot = m_XmlManager.GetDocument().DocumentElement;
				if (bIsNothing == false)
					pRoot.InsertAfter(m_pElement, node);
				else
					pRoot.AppendChild(m_pElement);

				PanelListLoad();
				if (bIsNothing == false)
					m_lbNpcSets.SelectedIndex = iIndex + 1;
				else
					m_lbNpcSets.SelectedIndex = 0;
			}
		}

		private void m_lbNpcSets_DoubleClick(object sender, EventArgs e)
		{
			int iIndex = m_lbNpcSets.SelectedIndex;
			if (iIndex == -1) return;
			XmlDocument pDoc = m_XmlManager.GetDocument();
			if (pDoc == null) return;
			XmlNodeList nodeList = pDoc.GetElementsByTagName("NPC");
			XmlNode node = nodeList.Item(iIndex);
			if (node.Attributes.GetNamedItem("NAME") == null)
				return;

			string sNpcName = node.Attributes.GetNamedItem("NAME").InnerText;
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
			ModifyNpc();
		}

		private void m_btnDelete_Click(object sender, EventArgs e)
		{
			int iIndex = m_lbNpcSets.SelectedIndex;
			if (iIndex == -1) return;
			XmlDocument pDoc = m_XmlManager.GetDocument();
			if (pDoc == null) return;
			XmlElement pRoot = m_XmlManager.GetDocument().DocumentElement;

			XmlNodeList nodeList = pDoc.GetElementsByTagName("NPC");
			XmlNode node = nodeList.Item(iIndex);
			pRoot.RemoveChild(node);
			PanelListLoad();

			if (iIndex >= nodeList.Count)
				m_lbNpcSets.SelectedIndex = nodeList.Count - 1;
			else
				m_lbNpcSets.SelectedIndex = iIndex;
		}

		private void m_btnCopy_Click(object sender, EventArgs e)
		{
			int iIndex = m_lbNpcSets.SelectedIndex;
			if (iIndex == -1) return;
			XmlDocument pDoc = m_XmlManager.GetDocument();
			if (pDoc == null) return;
			XmlElement pRoot = m_XmlManager.GetDocument().DocumentElement;

			XmlNodeList nodeList = pDoc.GetElementsByTagName("NPC");
			XmlNode node = nodeList.Item(iIndex);

			// Todo
			XmlNode nodeClone = node.Clone();
			nodeClone.Attributes.GetNamedItem("GUID").InnerText = Guid.NewGuid().ToString();			
			// Create Clone name
			bool bLoop = true;
			int iCount = 1;
			string strCloneName =
				nodeClone.Attributes.GetNamedItem("NAME").InnerText +
				"_" +
				iCount.ToString();
			while (bLoop)
			{
				for (int i = 0; i < nodeList.Count; i++)
				{
					string srcName = nodeList.Item(i).Attributes.GetNamedItem("NAME").InnerText;
					if (srcName.Equals(strCloneName))
					{
						iCount += 1;
						strCloneName =
							nodeClone.Attributes.GetNamedItem("NAME").InnerText +
							"_" +
							iCount.ToString();
					}
					else
					{
						bLoop = false;
					}
				}
			}
			// Create NPC Entity
			MPalette pkPalette = MFramework.Instance.PaletteManager.GetPaletteByName("General");
			if (pkPalette == null)
			{
				MessageBox.Show("General 팔레트가 없습니다.");
				return;
			}
			MEntity pkTemplate = pkPalette.GetEntityByName("[General]Target.npc_target");
			if (pkTemplate == null)
			{
				MessageBox.Show("[General]Target.npc_target 가 없습니다.");
				return;
			}
			MEntity pkNewEntity;
			pkNewEntity = pkTemplate.Clone(strCloneName, true);
			AddNpc(
				nodeClone.Attributes.GetNamedItem("LOCATION") != null ? nodeClone.Attributes.GetNamedItem("LOCATION").InnerText : null,
				nodeClone.Attributes.GetNamedItem("DIRECTION") != null ? nodeClone.Attributes.GetNamedItem("DIRECTION").InnerText : null,
				pkNewEntity);

			// Insert Node
			nodeClone.Attributes.GetNamedItem("NAME").InnerText = strCloneName;
			pRoot.InsertAfter(nodeClone, node);
			PanelListLoad();
			m_lbNpcSets.SelectedIndex = iIndex + 1;
		}

		private void m_btnClear_Click(object sender, EventArgs e)
		{
			Question frm = new Question();
			XmlElement pRoot = m_XmlManager.GetDocument().DocumentElement;
			if (DialogResult.OK == frm.ShowDialog())
			{
				XmlDocument pDoc = m_XmlManager.GetDocument();
				XmlNodeList nodeList = pDoc.GetElementsByTagName("NPC");
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
			int iIndex = m_lbNpcSets.SelectedIndex;
			if (iIndex == -1) return;
			XmlDocument pDoc = m_XmlManager.GetDocument();
			XmlNodeList nodeList = pDoc.GetElementsByTagName("NPC");

			if (iIndex <= 0 ||
				iIndex >= nodeList.Count)
				return;
			if (pDoc == null) return;
			XmlElement pRoot = m_XmlManager.GetDocument().DocumentElement;

			XmlNode node = nodeList.Item(iIndex - 1);
			XmlNode node2 = nodeList.Item(iIndex);

			SwapNode(nodeList, iIndex, node, node2);

			PanelListLoad();
			m_lbNpcSets.SelectedIndex = iIndex - 1;
		}

		private void m_btnMoveDown_Click(object sender, EventArgs e)
		{
			int iIndex = m_lbNpcSets.SelectedIndex;
			if (iIndex == -1) return;
			XmlDocument pDoc = m_XmlManager.GetDocument();
			XmlNodeList nodeList = pDoc.GetElementsByTagName("NPC");

			if (iIndex < 0 ||
				iIndex >= nodeList.Count - 1)
				return;
			if (pDoc == null) return;
			XmlElement pRoot = m_XmlManager.GetDocument().DocumentElement;

			XmlNode node = nodeList.Item(iIndex);
			XmlNode node2 = nodeList.Item(iIndex + 1);

			SwapNode(nodeList, iIndex, node, node2);

			PanelListLoad();
			m_lbNpcSets.SelectedIndex = iIndex + 1;
		}

		#endregion

		private void m_lbNpcSets_MouseDown(object sender, MouseEventArgs e)
		{
			if (e.Button.ToString().Equals("Right"))
			{
				ModifyNpc();
			}
		}
	}
}
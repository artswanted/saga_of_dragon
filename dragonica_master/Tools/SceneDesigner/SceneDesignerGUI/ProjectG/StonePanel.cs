using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Xml;
using System.Windows.Forms;
using System.Diagnostics;
using System.Collections;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;
using Emergent.Gamebryo.SceneDesigner.GUI.ProjectG;

namespace Emergent.Gamebryo.SceneDesigner.GUI.ProjectG
{
	public partial class StonePanel : Form
	{
		public class StoneList : object
		{
			public MEntity m_pkEntity = null;
			public float m_fRate = 0;
			public int m_iItemBagNo = 0;
		}

		private IUICommandService m_uiCommandService;
		// Xml 관리자
		private XmlManager m_XmlManager;
		//private XmlDocument m_XmlDocument;
		//private XmlElement m_pElement;
		private int m_iCurRate = 0;
		private int m_iCurItemBagNo = 0;
		private ArrayList m_pmStoneList = new ArrayList();  // StoneList의 리스트

		public StonePanel()
		{
			InitializeComponent();
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

		private static ISelectionService ms_pmSelectionService = null;
		private static ISelectionService SelectionService
		{
			get
			{
				if (ms_pmSelectionService == null)
				{
					ms_pmSelectionService = ServiceProvider.Instance
						.GetService(typeof(ISelectionService)) as
						ISelectionService;
					Debug.Assert(ms_pmSelectionService != null,
						"Selection service not found!");
				}
				return ms_pmSelectionService;
			}
		}

		public void SetXmlManager(ref XmlManager pXmlManager)
		{
			m_XmlManager = pXmlManager;
		}

		public void PanelListLoad()
		{
			// delete before
			for (int iIdx = 0; iIdx < m_pmStoneList.Count; iIdx++)
			{
				StoneList kStone = (StoneList)m_pmStoneList[iIdx];
				MEntity pkEntityInList = kStone.m_pkEntity;

				MFramework.Instance.Scene.RemoveEntity(pkEntityInList, false);
			}
			m_pmStoneList.Clear();
			m_lbStoneList.Items.Clear();

			XmlDocument pDoc = m_XmlManager.GetDocument();
			XmlElement pEle = GetStonesElement(m_XmlManager);
			if (pEle == null)
				return;

			int iResult;
			string strMin = pEle.GetAttribute("MINIMAM");
			if (int.TryParse(strMin.ToString(), out iResult))
			{
				m_tbMin.Text = strMin;
			}
			string strMax = pEle.GetAttribute("MAXIMAM");
			if (int.TryParse(strMax.ToString(), out iResult))
			{
				m_tbMax.Text = strMax;
			}

			// Create
			MPalette pkPalette = MFramework.Instance.PaletteManager.GetPaletteByName("General");
			if (pkPalette == null)
			{
				MessageBox.Show("General 팔레트가 없습니다.");
				return;
			}
			MEntity pkTemplate = pkPalette.GetEntityByName("[General]ShineStone.ShineStone");
			if (pkTemplate == null)
			{
				MessageBox.Show("[General]ShineStone.ShineStone 가 없습니다.");
				return;
			}
			//
			
			XmlNodeList nodeList = pDoc.GetElementsByTagName("STONE");
			for (int i = 0; i < nodeList.Count; i++)
			{
				XmlElement child = (XmlElement)nodeList.Item(i);
				if (child == null) continue;
				string strRate = child.GetAttribute("RATE");
				string strItemBagNo = child.GetAttribute("ITEMBAGNO");
				string strPosX = child.GetAttribute("POSX");
				string strPosY = child.GetAttribute("POSY");
				string strPosZ = child.GetAttribute("POSZ");

				float fRate = 0;
				float.TryParse(strRate.ToString(), out fRate);
				XML_LOCATION kLoc = new XML_LOCATION();
				
				float fResult;
				kLoc.fLocX = float.TryParse(strPosX.ToString(), out fResult) ? fResult : 0;
				kLoc.fLocY = float.TryParse(strPosY.ToString(), out fResult) ? fResult : 0;
				kLoc.fLocZ = float.TryParse(strPosZ.ToString(), out fResult) ? fResult : 0;

				// Create NPC Entity
				string strName = MFramework.Instance.Scene.GetUniqueEntityName(pkTemplate.Name);
				MEntity pkNewEntity;
				pkNewEntity = pkTemplate.Clone(strName, true);
				MPoint3 translation = new MPoint3(kLoc.fLocX, kLoc.fLocY, kLoc.fLocZ);
				pkNewEntity.SetPropertyData("Translation", translation, false);
				MFramework.Instance.Scene.AddEntity(pkNewEntity, false);

				// Add Stone List
				StoneList kStone = new StoneList();
				kStone.m_pkEntity = pkNewEntity;
				if (float.TryParse(strRate.ToString(), out fResult))
				{
					kStone.m_fRate = fResult;
				}
				int iItemBagNo = 0;
				kStone.m_iItemBagNo = int.TryParse(strItemBagNo.ToString(), out iItemBagNo) ? iItemBagNo : 0;
				m_pmStoneList.Add(kStone);

				// Add StoneList
				string strItem = StringForList(kStone.m_fRate.ToString(), kStone.m_iItemBagNo.ToString(), translation);
				m_lbStoneList.Items.Add(strItem);
			}
		}

		public XmlElement GetStonesElement(XmlManager kManager)
		{
			XmlElement kEle;

			XmlDocument pDoc = kManager.GetDocument();
			XmlElement pRoot = m_XmlManager.GetDocument().DocumentElement;
			XmlNodeList nodeList = pDoc.GetElementsByTagName("STONES");
			if (nodeList.Count != 0)
			{
				kEle = (XmlElement)nodeList.Item(0);
			}
			else
			{
				kEle = pDoc.CreateElement("STONES");
				pRoot.AppendChild(kEle);
			}

			return kEle;
		}

		public void SaveXML()
		{
			// Document 에 ID를 넣어주자.
			XmlDocument pDoc = m_XmlManager.GetDocument();
			XmlElement pEle = GetStonesElement(m_XmlManager);
			if (pEle == null) return;
			XmlNodeList nodeList = pEle.GetElementsByTagName("STONE");

			for (int i = 0; i < nodeList.Count; i++)
			{
				XmlElement node = (XmlElement)nodeList.Item(i);
				if (node == null) continue;
				node.SetAttribute("ID", (i+1).ToString());

				// 좌표 다시 갱신.
				StoneList kStone = (StoneList)m_pmStoneList[i];
				MEntity pkEntityInList = kStone.m_pkEntity;
				MPoint3 translation = pkEntityInList.GetPropertyData("Translation") as MPoint3;
				node.SetAttribute("POSX", translation.X.ToString());
				node.SetAttribute("POSY", translation.Y.ToString());
				node.SetAttribute("POSZ", translation.Z.ToString());

				// List Translate
				string strRate = node.GetAttribute("RATE");
				string strItemBagNo = node.GetAttribute("ITEMBAGNO");
				string strItem = StringForList(strRate, strItemBagNo, translation);
				m_lbStoneList.Items[i] = strItem;
			}
		}

		public string StringForList(string strRate, string strItemBagNo, MPoint3 kPos)
		{
			// Add Listbox
			string strItem = "Rate:" + strRate + ", ";
			strItem += "ItemBagNo:" + strItemBagNo + ", ";
			strItem += "Pos:" + kPos.X.ToString() + ",";
			strItem += kPos.Y.ToString() + ",";
			strItem += kPos.Z.ToString();

			return strItem;
		}

		public void AfterCreate(MEntity pkEntity)
		{
			// Add ListBox
			StoneList kStone = new StoneList();
			kStone.m_pkEntity = pkEntity;
			float fResult;
			if (float.TryParse(m_tbRate.ToString(), out fResult))
			{
				kStone.m_fRate = fResult;
			}
			int iItemBagNo = 0;
			kStone.m_iItemBagNo = int.TryParse(m_tbItemBagNo.ToString(), out iItemBagNo) ? iItemBagNo : 0;
			m_pmStoneList.Add(kStone);

			// Add StoneList
			MPoint3 translation = pkEntity.GetPropertyData("Translation") as MPoint3;
			string strItem = StringForList(kStone.m_fRate.ToString(), kStone.m_iItemBagNo.ToString(), translation);
			m_lbStoneList.Items.Add(strItem);
			
			MFramework.Instance.Scene.AddEntity(pkEntity, false);

			// Add Xml
			XmlDocument pDoc = m_XmlManager.GetDocument();
			XmlElement pRoot = m_XmlManager.GetDocument().DocumentElement;
			XmlElement pEle = GetStonesElement(m_XmlManager);

			XmlElement pChild;
			pChild = pDoc.CreateElement("STONE");
			pChild.SetAttribute("RATE", m_iCurRate.ToString());
			pChild.SetAttribute("POSX", translation.X.ToString());
			pChild.SetAttribute("POSY", translation.Y.ToString());
			pChild.SetAttribute("POSZ", translation.Z.ToString());
			pChild.SetAttribute("ITEMBAGNO", m_iCurRate.ToString());

			pEle.AppendChild(pChild);
		}

		public void AfterDelete(MEntity pkEntity)
		{
			int iIdx = 0;
			bool bDel = false;
			for (iIdx = 0; iIdx < m_pmStoneList.Count; iIdx++)
			{
				StoneList kStone = (StoneList)m_pmStoneList[iIdx];
				MEntity pkEntityInList = kStone.m_pkEntity;
				if (pkEntityInList != null &&
					pkEntityInList == pkEntity)
				{
					bDel = true;
					MFramework.Instance.Scene.RemoveEntity(pkEntity, false);
					break;
				}
			}

			if (!bDel)
				return;

			XmlDocument pDoc = m_XmlManager.GetDocument();
			XmlElement pEle = GetStonesElement(m_XmlManager);
			if (pEle == null) return;
			XmlNodeList nodeList = pEle.GetElementsByTagName("STONE");
			XmlNode node = nodeList.Item(iIdx);
			if (node == null) return;
			pEle.RemoveChild(node);

			// List Delete
			m_lbStoneList.Items.RemoveAt(iIdx);
		}

		public void AfterTranslate(MEntity pkEntity)
		{
			int iIdx = 0;
			bool bFind = false;
			for (iIdx = 0; iIdx < m_pmStoneList.Count; iIdx++)
			{
				StoneList kStone = (StoneList)m_pmStoneList[iIdx];
				MEntity pkEntityInList = kStone.m_pkEntity;
				if (pkEntityInList != null &&
					pkEntityInList == pkEntity)
				{
					bFind = true;
					break;
				}
			}

			if (!bFind)
				return;

			MPoint3 translation = pkEntity.GetPropertyData("Translation") as MPoint3;

			XmlDocument pDoc = m_XmlManager.GetDocument();
			XmlElement pEle = GetStonesElement(m_XmlManager);
			if (pEle == null) return;
			XmlNodeList nodeList = pEle.GetElementsByTagName("STONE");
			XmlElement node = (XmlElement)nodeList.Item(iIdx);
			if (node == null) return;
			node.SetAttribute("POSX", translation.X.ToString());
			node.SetAttribute("POSY", translation.Y.ToString());
			node.SetAttribute("POSZ", translation.Z.ToString());
			string strRate = node.GetAttribute("RATE");
			string strItemBagNo = node.GetAttribute("ITEMBAGNO");

			// List Translate
			string strItem = StringForList(strRate, strItemBagNo, translation);
			m_lbStoneList.Items[iIdx] = strItem;
		}

		public void AfterSelect(MEntity pkEntity)
		{
			for (int iIdx = 0; iIdx < m_pmStoneList.Count; iIdx++)
			{
				StoneList kStone = (StoneList)m_pmStoneList[iIdx];
				MEntity pkEntityInList = kStone.m_pkEntity;

				if (pkEntity == pkEntityInList && m_lbStoneList.Items.Count >= iIdx &&
					m_lbStoneList != null && m_lbStoneList.Items.Count > iIdx)
				{
					m_lbStoneList.SelectedIndex = iIdx;

					m_tbItemBagNo.Text = kStone.m_iItemBagNo.ToString();
					m_tbRate.Text = kStone.m_fRate.ToString();
					break;
				}
			}
		}

		private void m_btnCreate_Click(object sender, EventArgs e)
		{
			MPalette pkPalette = MFramework.Instance.PaletteManager.GetPaletteByName("General");
			if (pkPalette == null)
			{
				MessageBox.Show("General 팔레트가 없습니다.");
				return;
			}
			MEntity pkTemplate = pkPalette.GetEntityByName("[General]ShineStone.ShineStone");
			if (pkTemplate == null)
			{
				MessageBox.Show("[General]ShineStone.ShineStone 가 없습니다.");
				return;
			}
			MScene pmScene = MFramework.Instance.Scene;
			MFramework.Instance.PaletteManager.ActivePalette = pkPalette;
			if (MFramework.Instance.PaletteManager.ActivePalette.ActiveEntity !=
				pkTemplate)
			{
				MFramework.Instance.PaletteManager.ActivePalette.ActiveEntity = pkTemplate;
			}

			UICommand command = UICommandService.GetCommand("CreateInteractionMode");
			if (command != null)
			{
				command.DoClick(this, null);
			}
		}

		private void m_lbStoneList_DoubleClick(object sender, EventArgs e)
		{
			int iIndex = m_lbStoneList.SelectedIndex;
			if (iIndex == -1) return;

			StoneList kStone = (StoneList)m_pmStoneList[iIndex];

			MEntity[] pkEntities = new MEntity[1];
			pkEntities[0] = kStone.m_pkEntity;
			if (pkEntities[0] == null)
				return;
		
			ISelectionService pmSelectionService =
			   ServiceProvider.Instance.GetService(
			   typeof(ISelectionService)) as ISelectionService;
			pmSelectionService.ClearSelectedEntities();
			pmSelectionService.AddEntitiesToSelection(pkEntities);

			InteractionModeService.GetInteractionModeByName("GamebryoView").MouseMove(-999999, -999999);
		}

		private void m_tbMin_TextChanged(object sender, EventArgs e)
		{
			int iResult;
			if (int.TryParse(m_tbMin.Text, out iResult))
			{
				XmlDocument pDoc = m_XmlManager.GetDocument();
				XmlElement pEle = GetStonesElement(m_XmlManager);
				XmlElement pRoot = m_XmlManager.GetDocument().DocumentElement;
				pEle.SetAttribute("MINIMAM", iResult.ToString());
			}
		}

		private void m_tbMax_TextChanged(object sender, EventArgs e)
		{
			int iResult;
			if (int.TryParse(m_tbMax.Text, out iResult))
			{
				XmlDocument pDoc = m_XmlManager.GetDocument();
				XmlElement pEle = GetStonesElement(m_XmlManager);
				XmlElement pRoot = m_XmlManager.GetDocument().DocumentElement;
				pEle.SetAttribute("MAXIMAM", iResult.ToString());
			}
		}

		private void m_tbRate_TextChanged(object sender, EventArgs e)
		{
			int iResult;
			if (int.TryParse(m_tbRate.Text, out iResult))
			{
				m_iCurRate = iResult;

				// if selected entity count is 1 and type is stone
				MEntity[] amSelectedEntities = SelectionService.GetSelectedEntities();
				for(int i=0 ; i<amSelectedEntities.Length ; i++)
				//if (amSelectedEntities.Length == 1)
				{
					MEntity pkEntity = amSelectedEntities[i];
					for (int iIdx = 0; iIdx < m_pmStoneList.Count; iIdx++)
					{
						StoneList kStone = (StoneList)m_pmStoneList[iIdx];
						MEntity pkEntityInList = kStone.m_pkEntity;
						if (pkEntityInList != null &&
							pkEntityInList == pkEntity)
						{
							// Change value
							XmlDocument pDoc = m_XmlManager.GetDocument();
							XmlElement pEle = GetStonesElement(m_XmlManager);
							if (pEle == null) return;
							XmlNodeList nodeList = pEle.GetElementsByTagName("STONE");
							XmlElement node = (XmlElement)nodeList.Item(iIdx);
							if (node == null) return;

							node.SetAttribute("RATE", m_iCurRate.ToString());

							// Change ListBox
							MPoint3 translation = kStone.m_pkEntity.GetPropertyData("Translation") as MPoint3;
							string strItem = StringForList(m_iCurRate.ToString(), kStone.m_iItemBagNo.ToString(), translation);
							kStone.m_fRate = m_iCurRate;
							m_lbStoneList.Items[iIdx] = strItem;
							
							break;
						}
					}

				}
			}
		}

		private void m_tbItemBagNo_TextChanged(object sender, EventArgs e)
		{
			int iResult;
			if (int.TryParse(m_tbItemBagNo.Text, out iResult))
			{
				m_iCurItemBagNo = iResult;

				// if selected entity count is 1 and type is stone
				MEntity[] amSelectedEntities = SelectionService.GetSelectedEntities();
				for (int i = 0; i < amSelectedEntities.Length; i++)
				//if (amSelectedEntities.Length == 1)
				{
					MEntity pkEntity = amSelectedEntities[i]; 
					for (int iIdx = 0; iIdx < m_pmStoneList.Count; iIdx++)
					{
						StoneList kStone = (StoneList)m_pmStoneList[iIdx];
						MEntity pkEntityInList = kStone.m_pkEntity;
						if (pkEntityInList != null &&
							pkEntityInList == pkEntity)
						{
							// Change value
							XmlDocument pDoc = m_XmlManager.GetDocument();
							XmlElement pEle = GetStonesElement(m_XmlManager);
							if (pEle == null) return;
							XmlNodeList nodeList = pEle.GetElementsByTagName("STONE");
							XmlElement node = (XmlElement)nodeList.Item(iIdx);
							if (node == null) return;

							node.SetAttribute("ITEMBAGNO", m_iCurItemBagNo.ToString());

							// Change ListBox
							MPoint3 translation = kStone.m_pkEntity.GetPropertyData("Translation") as MPoint3;
							string strItem = StringForList(kStone.m_fRate.ToString(), m_iCurItemBagNo.ToString(), translation);
							kStone.m_iItemBagNo = m_iCurItemBagNo;
							m_lbStoneList.Items[iIdx] = strItem;

							break;
						}
					}

				}
			}
		}
	}
}
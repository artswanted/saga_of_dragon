using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using System.Collections;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;
using Emergent.Gamebryo.SceneDesigner.Framework;

namespace Emergent.Gamebryo.SceneDesigner.GUI.ProjectG
{
	public partial class MonsterArea : Form
	{
		private IUICommandService m_uiCommandService;
		private int m_iSelectedAreaID = -1;

		public MonsterArea()
		{
			InitializeComponent();

			UICommandService.BindCommands(this);

			m_tbAreaNumber.Text = "0";
			m_tbMonAreaID.Text = "MonAreaName";
			//MFramework.Instance.MonArea.SetMonAreaID(m_tbMonAreaID.Text);
			//int iResult;
			//if (Int32.TryParse(m_tbAreaNumber.Text, out iResult))
			//{
			//    MFramework.Instance.MonArea.SetAreaID(iResult);
			//}
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

		[UICommandHandler("Idle")]
		private void Application_Idle(object sender, EventArgs e)
		{
			MEntity[] amSelectedEntities = SelectionService.GetSelectedEntities();
			if (amSelectedEntities.Length == 1)
			{
				//int iSelectedAreaID = MFramework.Instance.MonArea.GetSelectedAreaID(amSelectedEntities[0]);
				//string strMonAreID = MFramework.Instance.MonArea.GetMonAreaID();
				//if (iSelectedAreaID != -1 &&
				//    m_iSelectedAreaID != iSelectedAreaID)
				//{
				//    m_iSelectedAreaID = iSelectedAreaID;
				//    m_tbAreaNumber.Text = m_iSelectedAreaID.ToString();
				//    m_tbMonAreaID.Text = strMonAreID;

				//    // 선택 된놈이다... 리스트 다 얻자.
				//    m_lbLinkArea.Items.Clear();
				//    ArrayList kLinkList = MFramework.Instance.MonArea.GetLinkAreaList(amSelectedEntities[0]);
				//    for (int i = 0; i < kLinkList.Count; ++i)
				//    {
				//        m_lbLinkArea.Items.Add(kLinkList[i].ToString());
				//    }
				//}
			}
			else
			{
				m_lbLinkArea.Items.Clear();
				m_iSelectedAreaID = -1;
			}
		}

		private void m_btnMakeDot_Click(object sender, EventArgs e)
		{
			MPalette pkPalette = MFramework.Instance.PaletteManager.GetPaletteByName("General");
			if (pkPalette == null)
			{
				MessageBox.Show("General 팔레트가 없습니다.");
				return;
			}
			MEntity pkTemplate = pkPalette.GetEntityByName("[General]Target.monarea_target");
			if (pkTemplate == null)
			{
				MessageBox.Show("[General]Target.monarea_target 가 없습니다.");
				return;
			}

			MScene pmScene = MFramework.Instance.Scene;

			MFramework.Instance.PaletteManager.ActivePalette = pkPalette;

			if (MFramework.Instance.PaletteManager.ActivePalette.ActiveEntity !=
				pkTemplate)
			{
				MFramework.Instance.PaletteManager.ActivePalette.ActiveEntity = pkTemplate;
			}

			UICommand command = UICommandService.GetCommand(
				"CreateInteractionMode");
			if (command != null)
			{
				command.DoClick(this, null);
			}
		}

		private void m_btnMake_Click(object sender, EventArgs e)
		{
			//MFramework.Instance.MonArea.MakeMonArea();
		}

		private void m_btnRevert_Click(object sender, EventArgs e)
		{
			//MFramework.Instance.MonArea.RevertPoint();
		}

		private void m_btnExtractPoint_Click(object sender, EventArgs e)
		{
			//MFramework.Instance.MonArea.ExtractPoint();
		}

		private void m_btnErasePoint_Click(object sender, EventArgs e)
		{
			//MFramework.Instance.MonArea.ErasePoint();
		}

		private void m_btnMakeCircle_Click(object sender, EventArgs e)
		{
			//MFramework.Instance.MonArea.MakeMonAreaCircle();
		}

		private void m_tbMonAreaID_TextChanged(object sender, EventArgs e)
		{
			//MFramework.Instance.MonArea.SetMonAreaID(m_tbMonAreaID.Text);
		}

		private void m_tbAreaNumber_TextChanged(object sender, EventArgs e)
		{
			int iResult;
			if (Int32.TryParse(m_tbAreaNumber.Text, out iResult))
			{
				//MFramework.Instance.MonArea.SetAreaID(iResult);
			}
		}

		private void m_lbLinkArea_MouseUp(object sender, MouseEventArgs e)
		{
			if (e.Button.Equals(MouseButtons.Right))
			{
				if (m_lbLinkArea.SelectedIndex >= 0)
				{
				}
				else
				{
				}

				this.m_cmLinkArea.Show(sender as Control, new Point(0, 0));
			}
		}

		private void addToolStripMenuItem_MouseUp(object sender, MouseEventArgs e)
		{
			if (e.Button.Equals(MouseButtons.Left))
			{
				MEntity[] amSelectedEntities = SelectionService.GetSelectedEntities();
				if (amSelectedEntities.Length == 1)
				{
					MonsterAreaDialog frm = new MonsterAreaDialog();
					frm.Text = "Add Link Area";
					if (DialogResult.OK == frm.ShowDialog())
					{
						int iResult = frm.m_iResult;
						string strID = iResult.ToString();
						// Add
						//ArrayList kLinkList = MFramework.Instance.MonArea.GetLinkAreaList(amSelectedEntities[0]);
						//if (kLinkList != null)
						//{
						//    kLinkList.Add(strID);
						//    m_lbLinkArea.Items.Add(strID);
						//}
					}
				}
			}
		}

		private void modifyToolStripMenuItem_MouseUp(object sender, MouseEventArgs e)
		{
			if (e.Button.Equals(MouseButtons.Left))
			{
				int iIdx = m_lbLinkArea.SelectedIndex;
				if (iIdx >= 0)
				{
					MEntity[] amSelectedEntities = SelectionService.GetSelectedEntities();
					if (amSelectedEntities.Length == 1)
					{
						string strOrgID = m_lbLinkArea.Items[iIdx].ToString();
						int iOrgResult;
						MonsterAreaDialog frm = new MonsterAreaDialog();
						frm.Text = "Modify Link Area";
						if (Int32.TryParse(strOrgID, out iOrgResult))
						{
							frm.m_iResult = iOrgResult;
							frm.Update();
						}
						else
						{
							return;
						}

						if (DialogResult.OK == frm.ShowDialog())
						{
							int iAfterResult = frm.m_iResult;
							string strAfterID = iAfterResult.ToString();

							// Modify
							//if (MFramework.Instance.MonArea.ModifyLinkAreaList(
							//    amSelectedEntities[0],
							//    iOrgResult,
							//    iAfterResult) == true)
							//{
							//    m_lbLinkArea.Items[iIdx] = strAfterID;
							//}
						}
					}
				}
			}
		}

		private void removeToolStripMenuItem_MouseUp(object sender, MouseEventArgs e)
		{
			if (e.Button.Equals(MouseButtons.Left))
			{
				int iIdx = m_lbLinkArea.SelectedIndex;
				if (iIdx >= 0)
				{
					MEntity[] amSelectedEntities = SelectionService.GetSelectedEntities();
					if (amSelectedEntities.Length == 1)
					{
						string strOrgID = m_lbLinkArea.Items[iIdx].ToString();
						int iOrgResult;
						if (Int32.TryParse(strOrgID, out iOrgResult))
						{
							// Remove
							//if (MFramework.Instance.MonArea.RemoveLinkAreaList(
							//    amSelectedEntities[0],
							//    iOrgResult) == true)
							//{
							//    m_lbLinkArea.Items.Remove(strOrgID);
							//}
						}
					}
				}
			}
		}
	}
}
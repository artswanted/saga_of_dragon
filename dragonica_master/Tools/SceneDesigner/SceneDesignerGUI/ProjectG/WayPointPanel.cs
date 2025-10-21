using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;
using Emergent.Gamebryo.SceneDesigner.Framework;

namespace Emergent.Gamebryo.SceneDesigner.GUI.ProjectG
{
	public partial class WayPointPanel : Form
	{
		private IUICommandService m_uiCommandService;

		public WayPointPanel()
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
		private void m_btnArray_Click(object sender, EventArgs e)
		{
			int iList = m_lbWayPointList.SelectedIndex;
			if (iList == -1)
			{
				MFramework.Instance.WayPoint.Array("");
			}
			else
			{
				MFramework.Instance.WayPoint.Array(m_lbWayPointList.Items[iList].ToString());
			}
		}

		private void m_btnAdd_Click(object sender, EventArgs e)
		{
			WayPointDialog frm = new WayPointDialog();
			if (DialogResult.OK == frm.ShowDialog())
			{
				String strGroupName = frm.m_strGroupName.ToString();
                strGroupName = strGroupName.Trim();
                if (0 == strGroupName.Length)
                {
                    return;
                }

				MFramework.Instance.WayPoint.AddGroup(strGroupName);

				int iList = m_lbWayPointList.FindString(strGroupName);
				if (iList == -1)
				{
					iList = m_lbWayPointList.Items.Add(strGroupName);
				}

				m_lbWayPointList.SelectedIndex = iList;

				MFramework.Instance.WayPoint.ChangeGroup(m_lbWayPointList.Items[iList].ToString());
			}
		}

		private void m_btnModify_Click(object sender, EventArgs e)
		{
			int iList = m_lbWayPointList.SelectedIndex;
			if (iList == -1)
				return;

			WayPointDialog frm = new WayPointDialog();
			frm.m_strGroupName = m_lbWayPointList.Items[iList].ToString();
			frm.Update();

			if (DialogResult.OK == frm.ShowDialog())
			{
				MFramework.Instance.WayPoint.ChangeGroupName(
					m_lbWayPointList.Items[iList].ToString(),
					frm.m_strGroupName.ToString()
					);
				MFramework.Instance.WayPoint.SetCurrentGroupName(frm.m_strGroupName.ToString());

				m_lbWayPointList.Items[iList] = frm.m_strGroupName.ToString();
			}
		}

		private void m_btnDel_Click(object sender, EventArgs e)
		{
			int iList = m_lbWayPointList.SelectedIndex;
			if (iList == -1)
				return;

			MFramework.Instance.WayPoint.DelGroup(m_lbWayPointList.Items[iList].ToString());
			m_lbWayPointList.Items.RemoveAt(iList);
		}

		private void m_lbWayPointList_DoubleClick(object sender, EventArgs e)
		{
			int iList = m_lbWayPointList.SelectedIndex;
			if (iList == -1)
				return;

			MFramework.Instance.WayPoint.SetCurrentGroupName(m_lbWayPointList.Items[iList].ToString());

			MPalette pkPalette = MFramework.Instance.PaletteManager.GetPaletteByName("General");
			if (pkPalette == null)
			{
				MessageBox.Show("General 팔레트가 없습니다.");
				return;
			}
			MEntity pkTemplate = pkPalette.GetEntityByName("[General]Target.waypoint_target");
			if (pkTemplate == null)
			{
				MessageBox.Show("[General]Target.waypoint_target 가 없습니다.");
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

		private void m_lbWayPointList_SelectedIndexChanged(object sender, EventArgs e)
		{
			UICommand command = UICommandService.GetCommand(
				"SelectInteractionMode");
			if (command != null)
			{
				command.DoClick(this, null);
			}

			// 선택한 것들만 show 다른건 hide
			int iList = m_lbWayPointList.SelectedIndex;
			if (iList == -1)
				return;

			MFramework.Instance.WayPoint.ChangeGroup(m_lbWayPointList.Items[iList].ToString());
		}

		private void m_btnChangeIndex_Click(object sender, EventArgs e)
		{
			int iIndex = 0;
			bool bCheck = int.TryParse(m_tbChangeIndex.Text, out iIndex);
			if (!bCheck)
			{
				return;
			}

			MEntity[] amSelectedEntities = SelectionService
				.GetSelectedEntities();

			for (int i = 0; i < amSelectedEntities.Length; i++)
			{
				MEntity pkEntity = amSelectedEntities[i];

				MFramework.Instance.WayPoint.ChangeIndex(pkEntity, iIndex);
			}
		}

		private void m_btnChangeRadius_Click(object sender, EventArgs e)
		{
			float fRadius = 0;
			bool bCheck = float.TryParse(m_tbChangeRadius.Text, out fRadius);
			if (!bCheck)
			{
				return;
			}

			MEntity[] amSelectedEntities = SelectionService
				.GetSelectedEntities();

			for (int i = 0; i < amSelectedEntities.Length; i++)
			{
				MEntity pkEntity = amSelectedEntities[i];

				MFramework.Instance.WayPoint.ChangeRadius(pkEntity, fRadius);
			}
		}

		[UICommandHandler("Idle")]
		private void Application_Idle(object sender, EventArgs e)
		{
			if (MFramework.Instance.WayPoint.m_bIsLoaded)
			{
				//로드 하자.
				m_lbWayPointList.Items.Clear();

				for (int i = 0;
					i < MFramework.Instance.WayPoint.GetGroupCount();
					i++)
				{
					String strName = MFramework.Instance.WayPoint.GetGroupName(i);
					m_lbWayPointList.Items.Add(strName);
				}

				MFramework.Instance.WayPoint.m_bIsLoaded = false;
			}
		}

		public void Clear()
		{
			m_lbWayPointList.Items.Clear();
			MFramework.Instance.WayPoint.Clear();
		}

		private void m_btnShowCircle_Click(object sender, EventArgs e)
		{
			MFramework.Instance.WayPoint.ShowCircle();
		}

		private void m_btnDeleteCircle_Click(object sender, EventArgs e)
		{
			MFramework.Instance.WayPoint.DeleteCircle();
		}

		private void m_btnApply_Click(object sender, EventArgs e)
		{
			MFramework.Instance.WayPoint.ApplyPosition();

			int iIndex = 0;
			bool bAbleIndex = int.TryParse(m_tbChangeIndex.Text, out iIndex);

			float fRadius = 0;
			bool bAbleRadius = float.TryParse(m_tbChangeRadius.Text, out fRadius);

			MEntity[] amSelectedEntities = SelectionService
				.GetSelectedEntities();
			for (int i = 0; i < amSelectedEntities.Length; i++)
			{
				MEntity pkEntity = amSelectedEntities[i];
				
				if (bAbleIndex)
				{
					MFramework.Instance.WayPoint.ChangeIndex(pkEntity, iIndex);
				}
				if (bAbleRadius)
				{
					MFramework.Instance.WayPoint.ChangeRadius(pkEntity, fRadius);
				}
			}
		}

        private void btnShow_Click(object sender, EventArgs e)
        {
            MFramework.Instance.WayPoint.SetEntityHide(false);
        }

        private void btnHide_Click(object sender, EventArgs e)
        {
            MFramework.Instance.WayPoint.SetEntityHide(true);
        }

   }
}
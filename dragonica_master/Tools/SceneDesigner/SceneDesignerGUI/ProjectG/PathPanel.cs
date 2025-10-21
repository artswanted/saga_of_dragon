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
	public partial class PathPanel : Form
	{
		private IUICommandService m_uiCommandService;

		//private float m_fWidthLeft = 60.0f;
		//private float m_fWidthRight = 60.0f;
		//private float m_fHeightLeft = 200.0f;
		//private float m_fHeightRight = 200.0f;
		//private bool m_bDrawLineLeft = false;
		//private bool m_bDrawLineRight = false;
		//private bool m_bDrawLineBottom = true;
		//private float m_iPathColorR = 128;
		//private float m_iPathColorG = 128;
		//private float m_iPathColorB = 255;
		//private float m_iPhysXColorR = 128;
		//private float m_iPhysXColorG = 255;
		//private float m_iPhysXColorB = 128;
		//private float m_fPointLength = 10.0f;

		private bool m_bDoMakeStraight = false;

		public PathPanel()
		{
			InitializeComponent();
			UICommandService.BindCommands(this);
			Initialize();
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

		private bool Initialize()
		{
			m_cbFace.SelectedIndex = 0;
			m_cbType.SelectedIndex = 0;

			m_btnCancelMakeStraight.Enabled = false;

			return true;
		}

		private void m_btnDraw_Click(object sender, EventArgs e)
		{
			MPalette pkPalette = MFramework.Instance.PaletteManager.GetPaletteByName("General");
			if (pkPalette == null)
			{
				MessageBox.Show("General 팔레트가 없습니다.");
				return;
			}
			MEntity pkTemplate = pkPalette.GetEntityByName("[General]Target.path_target");
			if (pkTemplate == null)
			{
				MessageBox.Show("[General]Target.path_target 가 없습니다.");
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

			return;
		}

		private void m_btnMakeUp_Click(object sender, EventArgs e)
		{
			float fHeight;
			if (!float.TryParse(m_tbHeight.Text, out fHeight))
			{
				fHeight = 500;
			}
			MFramework.Instance.Path.m_fHeight = fHeight;
			MFramework.Instance.Path.MakeUp();
		}

		private void m_btnMakeSide_Click(object sender, EventArgs e)
		{
			float fHeight;
			if (!float.TryParse(m_tbHeight.Text, out fHeight))
			{
				fHeight = 500;
			}
			MFramework.Instance.Path.m_fHeight = fHeight;

			if (m_cbFace.SelectedIndex == 0)
			{
				MFramework.Instance.Path.m_bIsMakeLeft = true;
			}
			else if (m_cbFace.SelectedIndex == 1)
			{
				MFramework.Instance.Path.m_bIsMakeLeft = false;
			}

			MFramework.Instance.Path.MakeSide();
		}

		private void m_btnExtract_Click(object sender, EventArgs e)
		{
			MFramework.Instance.Path.Clear();
			MFramework.Instance.Path.ExtractDot();
		}

		private void m_btnClear_Click(object sender, EventArgs e)
		{
			//패스 모두 지우기 선택시 실행여부 확인, 실수로 지우는 일 없도록
			if (MessageBox.Show("패스를 모두 지우시겠습니까?", "패스삭제 확인",
				MessageBoxButtons.OKCancel, MessageBoxIcon.Question)
				== System.Windows.Forms.DialogResult.OK)
			{
				MFramework.Instance.Path.Clear();
			}
		}

		private void m_btnMakeWall_Click(object sender, EventArgs e)
		{
			AddNewWall dlg = new AddNewWall();

			if (dlg.ShowDialog() == DialogResult.OK)
			{
				if (m_cbFace.SelectedIndex == 0)
				{
					MFramework.Instance.Path.m_bIsMakeLeft = true;
				}
				else if (m_cbFace.SelectedIndex == 1)
				{
					MFramework.Instance.Path.m_bIsMakeLeft = false;
				}

				if (m_cbType.SelectedIndex == 0)
				{
					MFramework.Instance.Path.m_bIsTypePath = true;
					MFramework.Instance.Path.m_bIsTypePhysX = false;
				}
				else if (m_cbType.SelectedIndex == 1)
				{
					MFramework.Instance.Path.m_bIsTypePath = true;
					MFramework.Instance.Path.m_bIsTypePhysX = true;
				}
				else if (m_cbType.SelectedIndex == 2)
				{
					MFramework.Instance.Path.m_bIsTypePath = false;
					MFramework.Instance.Path.m_bIsTypePhysX = true;
				}
				
				bool bCheck;
				int iMeshGroup = 1;
				bCheck = int.TryParse(m_tbMeshGroup.Text, out iMeshGroup);
				if (!bCheck)
				{
					iMeshGroup = 1;
				}
				MFramework.Instance.Path.m_iMeshGroup = iMeshGroup;
				MFramework.Instance.Path.MakeWall(dlg.m_strFilename, dlg.m_strSelectPalette);
			}
		}

		private void m_btnExtractPathNumber_Click(object sender, EventArgs e)
		{
			MFramework.Instance.Path.ModifyPath();
		}

		private void m_btnCancelMakeup_Click(object sender, EventArgs e)
		{
			MFramework.Instance.Path.CancelMakeUpSide();
		}

		private void m_btnMakeStraight_Click(object sender, EventArgs e)
		{
			MFramework.Instance.Path.MakeStraight();
			m_btnCancelMakeStraight.Enabled = true;
		}

		private void m_trbAnchorScale_Scroll(object sender, EventArgs e)
		{
			int iScale = m_trbAnchorScale.Value;
			float fResult = 0.019f*(float)iScale + 0.1f;
			MFramework.Instance.Path.SetAnchorScale(fResult);
		}

		private void m_btnCancelMakeStraight_Click(object sender, EventArgs e)
		{
			MFramework.Instance.Path.CancelMakeStraight();
			m_btnCancelMakeStraight.Enabled = false;
		}
	}
}

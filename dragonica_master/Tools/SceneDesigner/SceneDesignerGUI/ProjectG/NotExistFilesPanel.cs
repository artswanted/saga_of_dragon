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
	public partial class NotExistFilesPanel : Form
	{
		private IUICommandService m_uiCommandService;

		public NotExistFilesPanel()
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

		[UICommandHandler("Idle")]
		private void Application_Idle(object sender, EventArgs e)
		{
			if (MFramework.Instance.ExistFile.m_bToRefreshList)
			{
				MFramework.Instance.ExistFile.m_bToRefreshList = false;
				m_lbNotExistFilesList.Items.Clear();
				m_lbNotExistFilesList2.Items.Clear();

				for (uint i = 0;
					i < MFramework.Instance.ExistFile.GetNotExistFileCount();
					i++)
				{
					ExistFiles kFile = MFramework.Instance.ExistFile.GetNotExistFile(i, false);
					if (kFile != null &&
						kFile.m_pkEntity != null &&
						kFile.m_pkEntity.MasterEntity != null)
					{
						string strData = kFile.m_pkEntity.MasterEntity.Name;
						strData += "   ??   ";
						strData += kFile.m_pkFilePath;

						m_lbNotExistFilesList.Items.Add(strData);
					}
				}

				for (uint i = 0;
					i < MFramework.Instance.ExistFile.GetNotExistFile2Count();
					i++)
				{
					ExistFiles kFile = MFramework.Instance.ExistFile.GetNotExistFile2(i, false);
					if (kFile != null &&
						kFile.m_pkEntity != null)
					{
						string strData = kFile.m_pkEntity.Name;
						strData += "   ??   ";
						strData += kFile.m_pkFilePath;

						m_lbNotExistFilesList2.Items.Add(strData);
					}
				}
			}
		}

		private void m_lbNotExistFilesList_DoubleClick(object sender, EventArgs e)
		{
			int iIdx = m_lbNotExistFilesList.SelectedIndex;
			if (iIdx != -1)
			{
				ExistFiles kFile = MFramework.Instance.ExistFile.GetNotExistFile((uint)iIdx, false);

				if (kFile != null &&
					kFile.m_pkEntity != null &&
					kFile.m_pkEntity.MasterEntity != null)
				{
					ServiceProvider sp = ServiceProvider.Instance;
					IEntityPathService pathService = sp.GetService(
						typeof(IEntityPathService)) as IEntityPathService;
					MPalette palette =
						pathService.FindPaletteContainingEntity(
						kFile.m_pkEntity.MasterEntity);

					ICommandPanelService panelService =
						ServiceProvider.Instance.GetService(
						typeof(ICommandPanelService)) as ICommandPanelService;
					if (palette != null)
					{
						MFramework.Instance.PaletteManager.ActivePalette =
							palette;
						palette.ActiveEntity = kFile.m_pkEntity.MasterEntity;

						Form panel = panelService.GetPanel("Template Properties");
						if (panel != null)
						{
							panelService.ShowPanel("Template Properties", true);
						}
					}
					else
					{
						SelectionService.SelectedEntity = kFile.m_pkEntity;
						Form panel = panelService.GetPanel("Entity Properties");
						if (panel != null)
						{
							panelService.ShowPanel("Entity Properties", true);
						}
					}
				}
			}
		}

		private void m_lbNotExistFilesList2_DoubleClick(object sender, EventArgs e)
		{
			int iIdx = m_lbNotExistFilesList2.SelectedIndex;
			if (iIdx != -1)
			{
				ExistFiles kFile = MFramework.Instance.ExistFile.GetNotExistFile2((uint)iIdx, false);

				if (kFile != null &&
					kFile.m_pkEntity != null)
				{
					ServiceProvider sp = ServiceProvider.Instance;
					IEntityPathService pathService = sp.GetService(
						typeof(IEntityPathService)) as IEntityPathService;
					MPalette palette =
						pathService.FindPaletteContainingEntity(
						kFile.m_pkEntity);
					if (palette != null)
					{
						MFramework.Instance.PaletteManager.ActivePalette =
							palette;
						palette.ActiveEntity = kFile.m_pkEntity;

						ICommandPanelService panelService =
							ServiceProvider.Instance.GetService(
							typeof(ICommandPanelService)) as ICommandPanelService;
						Form panel = panelService.GetPanel("Template Properties");
						if (panel != null)
						{
							panelService.ShowPanel("Template Properties", true);
						}
					}
				}
			}
		}

		private void m_btnRefresh_Click(object sender, EventArgs e)
		{
			m_lbNotExistFilesList.Items.Clear();
			m_lbNotExistFilesList2.Items.Clear();
			MFramework.Instance.ExistFile.ClearNotExistFile();
			MFramework.Instance.ExistFile.FindNotExistFile();
		}

		private void m_btnRemove_Click(object sender, EventArgs e)
		{
			DialogResult result = MessageBox.Show(
				"존재 하지 않는 물체를 지웁니다. 지우시겠습니까?",
				"경고",
				MessageBoxButtons.OKCancel);
			if (result != DialogResult.OK)
			{
				return;
			}

			for (int i = 0; i < m_lbNotExistFilesList.Items.Count; i++)
			{
				ExistFiles kFile = MFramework.Instance.ExistFile.GetNotExistFile((uint)0, true);

				if (kFile != null &&
					kFile.m_pkEntity != null &&
					kFile.m_pkEntity.MasterEntity != null)
				{
					MFramework.Instance.Scene.RemoveEntity(kFile.m_pkEntity, true);
				}
			}
			m_btnRefresh_Click(sender, e);
		}

		private void m_btnRemove2_Click(object sender, EventArgs e)
		{
			DialogResult result = MessageBox.Show(
				"존재 하지 않는 물체를 지웁니다. 지우시겠습니까?",
				"경고",
				MessageBoxButtons.OKCancel);
			if (result != DialogResult.OK)
			{
				return;
			}

			for (int i = 0; i < m_lbNotExistFilesList2.Items.Count; i++)
			{
				ExistFiles kFile = MFramework.Instance.ExistFile.GetNotExistFile2((uint)0, true);

				if (kFile != null &&
					kFile.m_pkEntity != null)
				{
					ServiceProvider sp = ServiceProvider.Instance;
					IEntityPathService pathService = sp.GetService(
						typeof(IEntityPathService)) as IEntityPathService;
					MPalette palette =
						pathService.FindPaletteContainingEntity(
						kFile.m_pkEntity);
					if (palette != null)
					{
						palette.RemoveEntity(kFile.m_pkEntity, true);
					}
				}
			}
			m_btnRefresh_Click(sender, e);
		}

		private void m_btnRemoveOne_Click(object sender, EventArgs e)
		{
			DialogResult result = MessageBox.Show(
				"존재 하지 않는 물체를 지웁니다. 지우시겠습니까?",
				"경고",
				MessageBoxButtons.OKCancel);
			if (result != DialogResult.OK)
			{
				return;
			}

			int iIdx = m_lbNotExistFilesList2.SelectedIndex;
			if (iIdx != -1)
			{
				ExistFiles kFile = MFramework.Instance.ExistFile.GetNotExistFile((uint)0, true);

				if (kFile != null &&
					kFile.m_pkEntity != null &&
					kFile.m_pkEntity.MasterEntity != null)
				{
					MFramework.Instance.Scene.RemoveEntity(kFile.m_pkEntity, true);
					m_btnRefresh_Click(sender, e);
				}
			}
		}

		private void m_btnRemoveOne2_Click(object sender, EventArgs e)
		{
			DialogResult result = MessageBox.Show(
				"존재 하지 않는 물체를 지웁니다. 지우시겠습니까?",
				"경고",
				MessageBoxButtons.OKCancel);
			if (result != DialogResult.OK)
			{
				return;
			}

			int iIdx = m_lbNotExistFilesList2.SelectedIndex;
			if (iIdx != -1)
			{
				ExistFiles kFile = MFramework.Instance.ExistFile.GetNotExistFile2((uint)iIdx, true);
				if (kFile != null && 
					kFile.m_pkEntity != null)
				{
					ServiceProvider sp = ServiceProvider.Instance;
					IEntityPathService pathService = sp.GetService(
						typeof(IEntityPathService)) as IEntityPathService;
					MPalette palette =
						pathService.FindPaletteContainingEntity(
						kFile.m_pkEntity);
					if (palette != null)
					{
						palette.RemoveEntity(kFile.m_pkEntity, true);
					}
					m_btnRefresh_Click(sender, e);
				}
			}
		}

	}
}
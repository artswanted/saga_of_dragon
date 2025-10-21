// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//	  Copyright (c) 1996-2006 Emergent Game Technologies.
//	  All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;
using System.Diagnostics;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using PluginAPI = Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;

namespace Emergent.Gamebryo.SceneDesigner.GUI
{
	public class SelectionPanel : Form
	{
		#region Private Data
		//the following two variables are to prevent the two selection 
		//handlers for the GUI and the manager from handling events
		//that were triggered by the other function
		//the will prevent a stack overflow 
		//private bool m_isInManagerHandler = false;
		//private bool m_isInGuiHandler = false;
		private bool m_bSynchingSelectionService = false;
		private bool m_bSynchingListBox = false;
		private bool m_bEnableListBoxSelectedIndexChanged = true;
		private MEntity[] m_amEntitiesSelectedAtLastSync = null;
		private EntityComparer m_pmEntityComparer = new EntityComparer();
		private ArrayList m_pmEntityList = new ArrayList();
		private bool m_bTextChangeCase = false;

		private enum FilterState
		{
			Visible,
			Hidden,
			Frozen
		};
		private FilterState m_eFilterState = FilterState.Visible;

		private static string ms_strHiddenFilterSettingName =
			"DisplayHiddenFilterWarning";
		private static string ms_strFrozenFilterSettingName =
			"DisplayFrozenFilterWarning";
		#endregion

		private System.Windows.Forms.ListBox m_lbEntityList;
		private System.Windows.Forms.TextBox m_tbEntityName;
		private System.Windows.Forms.Label m_lblSelectionSet;
		private System.Windows.Forms.ComboBox m_cbSelectionSets;
		private System.Windows.Forms.ToolBar m_tbToolbar;
		private System.Windows.Forms.ToolBarButton m_tbbHideUnhide;
		private System.Windows.Forms.ToolBarButton m_tbbFreezeUnfreeze;
		private System.Windows.Forms.ToolBarButton m_tbbSeparator1;
		private System.Windows.Forms.ToolBarButton m_tbbSelectAll;
		private System.Windows.Forms.ToolBarButton m_tbbSelectNone;
		private System.Windows.Forms.ToolBarButton m_tbbInvertSelection;
		private System.Windows.Forms.ToolBarButton m_tbbSeparator2;
		private System.Windows.Forms.ToolBarButton m_tbbFilter;
		private System.Windows.Forms.ImageList m_ilToobarImages;
		private System.Windows.Forms.ContextMenu m_menuContext;
		private System.Windows.Forms.MenuItem m_menuHide;
		private System.Windows.Forms.MenuItem m_MenuFreeze;
		private System.Windows.Forms.MenuItem m_menuFindPalette;
		private System.Windows.Forms.MenuItem menuItem1;
		private ToolBarButton m_tbbSeparator3;
		private ToolBarButton m_tbbCopyList;
		private ToolBarButton m_tbbSelectFromClip;
		private System.ComponentModel.IContainer components = null;

		public SelectionPanel(IUICommandService commandService)
		{
			// This call is required by the Windows Form Designer.
			InitializeComponent();
			commandService.BindCommands(this);
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(SelectionPanel));
			this.m_lbEntityList = new System.Windows.Forms.ListBox();
			this.m_tbEntityName = new System.Windows.Forms.TextBox();
			this.m_lblSelectionSet = new System.Windows.Forms.Label();
			this.m_cbSelectionSets = new System.Windows.Forms.ComboBox();
			this.m_tbToolbar = new System.Windows.Forms.ToolBar();
			this.m_tbbSelectAll = new System.Windows.Forms.ToolBarButton();
			this.m_tbbSelectNone = new System.Windows.Forms.ToolBarButton();
			this.m_tbbInvertSelection = new System.Windows.Forms.ToolBarButton();
			this.m_tbbSeparator1 = new System.Windows.Forms.ToolBarButton();
			this.m_tbbHideUnhide = new System.Windows.Forms.ToolBarButton();
			this.m_tbbFreezeUnfreeze = new System.Windows.Forms.ToolBarButton();
			this.m_tbbSeparator2 = new System.Windows.Forms.ToolBarButton();
			this.m_tbbFilter = new System.Windows.Forms.ToolBarButton();
			this.m_tbbSeparator3 = new System.Windows.Forms.ToolBarButton();
			this.m_tbbCopyList = new System.Windows.Forms.ToolBarButton();
			this.m_tbbSelectFromClip = new System.Windows.Forms.ToolBarButton();
			this.m_ilToobarImages = new System.Windows.Forms.ImageList(this.components);
			this.m_menuContext = new System.Windows.Forms.ContextMenu();
			this.menuItem1 = new System.Windows.Forms.MenuItem();
			this.m_menuHide = new System.Windows.Forms.MenuItem();
			this.m_MenuFreeze = new System.Windows.Forms.MenuItem();
			this.m_menuFindPalette = new System.Windows.Forms.MenuItem();
			this.SuspendLayout();
			// 
			// m_lbEntityList
			// 
			this.m_lbEntityList.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.m_lbEntityList.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
			this.m_lbEntityList.IntegralHeight = false;
			this.m_lbEntityList.Location = new System.Drawing.Point(10, 60);
			this.m_lbEntityList.Name = "m_lbEntityList";
			this.m_lbEntityList.SelectionMode = System.Windows.Forms.SelectionMode.MultiExtended;
			this.m_lbEntityList.Size = new System.Drawing.Size(340, 312);
			this.m_lbEntityList.TabIndex = 2;
			this.m_lbEntityList.DrawItem += new System.Windows.Forms.DrawItemEventHandler(this.m_lbEntityList_DrawItem);
			this.m_lbEntityList.DoubleClick += new System.EventHandler(this.m_lbEntityList_DoubleClick);
			this.m_lbEntityList.SelectedIndexChanged += new System.EventHandler(this.m_lbEntityList_SelectedIndexChanged);
			this.m_lbEntityList.MouseUp += new System.Windows.Forms.MouseEventHandler(this.m_lbEntityList_MouseUp);
			this.m_lbEntityList.KeyDown += new System.Windows.Forms.KeyEventHandler(this.OnKeyDown);
			// 
			// m_tbEntityName
			// 
			this.m_tbEntityName.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.m_tbEntityName.Location = new System.Drawing.Point(10, 34);
			this.m_tbEntityName.Multiline = true;
			this.m_tbEntityName.Name = "m_tbEntityName";
			this.m_tbEntityName.Size = new System.Drawing.Size(340, 21);
			this.m_tbEntityName.TabIndex = 1;
			this.m_tbEntityName.TextChanged += new System.EventHandler(this.m_tbEntityName_TextChanged);
			// 
			// m_lblSelectionSet
			// 
			this.m_lblSelectionSet.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.m_lblSelectionSet.AutoSize = true;
			this.m_lblSelectionSet.Location = new System.Drawing.Point(10, 381);
			this.m_lblSelectionSet.Name = "m_lblSelectionSet";
			this.m_lblSelectionSet.Size = new System.Drawing.Size(83, 12);
			this.m_lblSelectionSet.TabIndex = 3;
			this.m_lblSelectionSet.Text = "Selection Set:";
			// 
			// m_cbSelectionSets
			// 
			this.m_cbSelectionSets.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.m_cbSelectionSets.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.m_cbSelectionSets.Location = new System.Drawing.Point(10, 398);
			this.m_cbSelectionSets.Name = "m_cbSelectionSets";
			this.m_cbSelectionSets.Size = new System.Drawing.Size(340, 20);
			this.m_cbSelectionSets.Sorted = true;
			this.m_cbSelectionSets.TabIndex = 4;
			this.m_cbSelectionSets.SelectedIndexChanged += new System.EventHandler(this.m_cbSelectionSets_SelectedIndexChanged);
			// 
			// m_tbToolbar
			// 
			this.m_tbToolbar.Appearance = System.Windows.Forms.ToolBarAppearance.Flat;
			this.m_tbToolbar.Buttons.AddRange(new System.Windows.Forms.ToolBarButton[] {
            this.m_tbbSelectAll,
            this.m_tbbSelectNone,
            this.m_tbbInvertSelection,
            this.m_tbbSeparator1,
            this.m_tbbHideUnhide,
            this.m_tbbFreezeUnfreeze,
            this.m_tbbSeparator2,
            this.m_tbbFilter,
            this.m_tbbSeparator3,
            this.m_tbbCopyList,
            this.m_tbbSelectFromClip});
			this.m_tbToolbar.Divider = false;
			this.m_tbToolbar.DropDownArrows = true;
			this.m_tbToolbar.ImageList = this.m_ilToobarImages;
			this.m_tbToolbar.Location = new System.Drawing.Point(0, 0);
			this.m_tbToolbar.Name = "m_tbToolbar";
			this.m_tbToolbar.ShowToolTips = true;
			this.m_tbToolbar.Size = new System.Drawing.Size(360, 26);
			this.m_tbToolbar.TabIndex = 0;
			this.m_tbToolbar.ButtonClick += new System.Windows.Forms.ToolBarButtonClickEventHandler(this.m_tbToolbar_ButtonClick);
			this.m_tbToolbar.ButtonDropDown += new System.Windows.Forms.ToolBarButtonClickEventHandler(this.m_tbToolbar_ButtonDropDown);
			// 
			// m_tbbSelectAll
			// 
			this.m_tbbSelectAll.ImageIndex = 0;
			this.m_tbbSelectAll.Name = "m_tbbSelectAll";
			this.m_tbbSelectAll.ToolTipText = "Select All";
			// 
			// m_tbbSelectNone
			// 
			this.m_tbbSelectNone.ImageIndex = 1;
			this.m_tbbSelectNone.Name = "m_tbbSelectNone";
			this.m_tbbSelectNone.ToolTipText = "Select None";
			// 
			// m_tbbInvertSelection
			// 
			this.m_tbbInvertSelection.ImageIndex = 2;
			this.m_tbbInvertSelection.Name = "m_tbbInvertSelection";
			this.m_tbbInvertSelection.ToolTipText = "Invert Selection";
			// 
			// m_tbbSeparator1
			// 
			this.m_tbbSeparator1.Name = "m_tbbSeparator1";
			this.m_tbbSeparator1.Style = System.Windows.Forms.ToolBarButtonStyle.Separator;
			// 
			// m_tbbHideUnhide
			// 
			this.m_tbbHideUnhide.ImageIndex = 3;
			this.m_tbbHideUnhide.Name = "m_tbbHideUnhide";
			// 
			// m_tbbFreezeUnfreeze
			// 
			this.m_tbbFreezeUnfreeze.ImageIndex = 4;
			this.m_tbbFreezeUnfreeze.Name = "m_tbbFreezeUnfreeze";
			// 
			// m_tbbSeparator2
			// 
			this.m_tbbSeparator2.Name = "m_tbbSeparator2";
			this.m_tbbSeparator2.Style = System.Windows.Forms.ToolBarButtonStyle.Separator;
			// 
			// m_tbbFilter
			// 
			this.m_tbbFilter.Name = "m_tbbFilter";
			this.m_tbbFilter.Style = System.Windows.Forms.ToolBarButtonStyle.DropDownButton;
			// 
			// m_tbbSeparator3
			// 
			this.m_tbbSeparator3.Name = "m_tbbSeparator3";
			this.m_tbbSeparator3.Style = System.Windows.Forms.ToolBarButtonStyle.Separator;
			// 
			// m_tbbCopyList
			// 
			this.m_tbbCopyList.ImageIndex = 0;
			this.m_tbbCopyList.Name = "m_tbbCopyList";
			this.m_tbbCopyList.ToolTipText = "Copy List to Clipboard";
			// 
			// m_tbbSelectFromClip
			// 
			this.m_tbbSelectFromClip.ImageIndex = 7;
			this.m_tbbSelectFromClip.Name = "m_tbbSelectFromClip";
			this.m_tbbSelectFromClip.ToolTipText = "Select Entities from clipboard";
			// 
			// m_ilToobarImages
			// 
			this.m_ilToobarImages.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("m_ilToobarImages.ImageStream")));
			this.m_ilToobarImages.TransparentColor = System.Drawing.Color.Fuchsia;
			this.m_ilToobarImages.Images.SetKeyName(0, "");
			this.m_ilToobarImages.Images.SetKeyName(1, "");
			this.m_ilToobarImages.Images.SetKeyName(2, "");
			this.m_ilToobarImages.Images.SetKeyName(3, "");
			this.m_ilToobarImages.Images.SetKeyName(4, "");
			this.m_ilToobarImages.Images.SetKeyName(5, "");
			this.m_ilToobarImages.Images.SetKeyName(6, "");
			this.m_ilToobarImages.Images.SetKeyName(7, "");
			// 
			// m_menuContext
			// 
			this.m_menuContext.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
            this.menuItem1,
            this.m_menuHide,
            this.m_MenuFreeze,
            this.m_menuFindPalette});
			// 
			// menuItem1
			// 
			this.menuItem1.Index = 0;
			this.menuItem1.Text = "Delete";
			// 
			// m_menuHide
			// 
			this.m_menuHide.Index = 1;
			this.m_menuHide.Text = "Hide";
			// 
			// m_MenuFreeze
			// 
			this.m_MenuFreeze.Index = 2;
			this.m_MenuFreeze.Text = "Freeze";
			// 
			// m_menuFindPalette
			// 
			this.m_menuFindPalette.Index = 3;
			this.m_menuFindPalette.Text = "Find Palette";
			// 
			// SelectionPanel
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(6, 14);
			this.AutoScroll = true;
			this.ClientSize = new System.Drawing.Size(360, 430);
			this.Controls.Add(this.m_tbToolbar);
			this.Controls.Add(this.m_cbSelectionSets);
			this.Controls.Add(this.m_lblSelectionSet);
			this.Controls.Add(this.m_tbEntityName);
			this.Controls.Add(this.m_lbEntityList);
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.KeyPreview = true;
			this.Name = "SelectionPanel";
			this.Text = "Selection";
			this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.OnKeyDown);
			this.Load += new System.EventHandler(this.SelectionPanel_Load);
			this.ResumeLayout(false);
			this.PerformLayout();

		}
		#endregion

		#region EventManager Handlers
		private bool m_bBuildAndSync = false;
		private bool m_bSyncListBoxToSelectionService = false;
		private bool m_bSyncHideFreezeButtons = false;
		private bool m_bSyncSelectionSetComboBox = false;

		private void EventManager_NewSceneLoaded(MScene pmScene)
		{
			if (pmScene == MFramework.Instance.Scene)
			{
				if (!MFramework.Instance.PerformingLongOperation)
				{
					BuildAndSync();
					SyncSelectionSetComboBox();
				}
				else
				{
					m_bBuildAndSync = true;
					m_bSyncSelectionSetComboBox = true;
				}
			}
		}

		private void EventManager_SelectedEntitiesChanged()
		{
			if (!MFramework.Instance.PerformingLongOperation)
			{
				SyncListBoxToSelectionService();
				SyncHideFreezeButtons();
			}
			else
			{
				m_bSyncListBoxToSelectionService = true;
				m_bSyncHideFreezeButtons = true;
			}
		}

		private void EventManager_EntityAddedToScene(MScene pmScene,
			MEntity pmEntity)
		{
			if (pmScene == MFramework.Instance.Scene)
			{
				if (!MFramework.Instance.PerformingLongOperation)
				{
					BuildAndSync();
				}
				else
				{
					m_bBuildAndSync = true;
				}
			}
		}

		private void EventManager_EntityRemovedFromScene(MScene pmScene,
			MEntity pmEntity)
		{
			if (pmScene == MFramework.Instance.Scene)
			{
				if (!MFramework.Instance.PerformingLongOperation)
				{
					BuildAndSync();
				}
				else
				{
					m_bBuildAndSync = true;
				}
			}
		}

		private void EventManager_EntityNameChanged(MEntity pmEntity,
			string strOldName, bool bInBatch)
		{
			if (MFramework.Instance.Scene.IsEntityInScene(pmEntity))
			{
				if (!MFramework.Instance.PerformingLongOperation)
				{
					BuildAndSync();
				}
				else
				{
					m_bBuildAndSync = true;
				}
			}
		}

		private void EventManager_EntityHiddenStateChanged(MEntity pmEntity,
			bool bHidden)
		{
			if (MFramework.Instance.Scene.IsEntityInScene(pmEntity))
			{
				if (!MFramework.Instance.PerformingLongOperation)
				{
					BuildAndSync();
				}
				else
				{
					m_bBuildAndSync = true;
				}
			}
		}

		private void EventManager_EntityFrozenStateChanged(MEntity pmEntity,
			bool bFrozen)
		{
			if (MFramework.Instance.Scene.IsEntityInScene(pmEntity))
			{
				if (!MFramework.Instance.PerformingLongOperation)
				{
					BuildAndSync();
				}
				else
				{
					m_bBuildAndSync = true;
				}
			}
		}

		private void EventManager_SelectionSetAddedToScene(MScene pmScene,
			MSelectionSet pmSelectionSet)
		{
			if (MFramework.Instance.Scene == pmScene)
			{
				if (!MFramework.Instance.PerformingLongOperation)
				{
					SyncSelectionSetComboBox();
				}
				else
				{
					m_bSyncSelectionSetComboBox = true;
				}
			}
		}

		private void EventManager_SelectionSetRemovedFromScene(MScene pmScene,
			MSelectionSet pmSelectionSet)
		{
			if (MFramework.Instance.Scene == pmScene)
			{
				if (!MFramework.Instance.PerformingLongOperation)
				{
					SyncSelectionSetComboBox();
				}
				else
				{
					m_bSyncSelectionSetComboBox = true;
				}
			}
		}

		private void EventManager_SelectionSetNameChanged(
			MSelectionSet pmSelectionSet, string strOldName, bool bInBatch)
		{
			if (MFramework.Instance.Scene.IsSelectionSetInScene(
				pmSelectionSet))
			{
				if (!MFramework.Instance.PerformingLongOperation)
				{
					SyncSelectionSetComboBox();
				}
				else
				{
					m_bSyncSelectionSetComboBox = true;
				}
			}
		}

		private void EventManager_LongOperationCompleted()
		{
			if (m_bBuildAndSync)
			{
				BuildAndSync();
			}
			else if (m_bSyncListBoxToSelectionService)
			{
				SyncListBoxToSelectionService();
			}

			if (m_bSyncHideFreezeButtons)
			{
				SyncHideFreezeButtons();
			}

			if (m_bSyncSelectionSetComboBox)
			{
				SyncSelectionSetComboBox();
			}

			m_bBuildAndSync = false;
			m_bSyncListBoxToSelectionService = false;
			m_bSyncHideFreezeButtons = false;
			m_bSyncSelectionSetComboBox = false;
		}
		#endregion

		#region Service Accessors
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

		private static ICommandService ms_pmCommandService = null;
		private static ICommandService CommandService
		{
			get
			{
				if (ms_pmCommandService == null)
				{
					ms_pmCommandService = ServiceProvider.Instance
						.GetService(typeof(ICommandService)) as
						ICommandService;
					Debug.Assert(ms_pmCommandService != null,
						"Command service not found!");
				}
				return ms_pmCommandService;
			}
		}

		private static IMessageService ms_pmMessageService = null;
		private static IMessageService MessageService
		{
			get
			{
				if (ms_pmMessageService == null)
				{
					ms_pmMessageService = ServiceProvider.Instance
						.GetService(typeof(IMessageService)) as
						IMessageService;
					Debug.Assert(ms_pmMessageService != null,
						"Message service not found!");
				}
				return ms_pmMessageService;
			}
		}

		private static ISelectionSetService ms_pmSelectionSetService = null;
		private static ISelectionSetService SelectionSetService
		{
			get
			{
				if (ms_pmSelectionSetService == null)
				{
					ms_pmSelectionSetService = ServiceProvider.Instance
						.GetService(typeof(ISelectionSetService)) as
						ISelectionSetService;
					Debug.Assert(ms_pmSelectionSetService != null,
						"Selection set service not found!");
				}
				return ms_pmSelectionSetService;
			}
		}

		private static ISettingsService ms_pmSettingsService = null;
		private static ISettingsService SettingsService
		{
			get
			{
				if (ms_pmSettingsService == null)
				{
					ms_pmSettingsService = ServiceProvider.Instance
						.GetService(typeof(ISettingsService)) as
						ISettingsService;
					Debug.Assert(ms_pmSettingsService != null,
						"Settings service not found!");
				}
				return ms_pmSettingsService;
			}
		}
		#endregion

		#region Private Helpers
		private void AttachToEventManager()
		{
			MEventManager pmEventMgr = MFramework.Instance.EventManager;
			pmEventMgr.SelectedEntitiesChanged += 
				new MEventManager.__Delegate_SelectedEntitiesChanged(
				EventManager_SelectedEntitiesChanged);
			pmEventMgr.EntityAddedToScene += 
				new MEventManager.__Delegate_EntityAddedToScene(
				EventManager_EntityAddedToScene);
			pmEventMgr.EntityRemovedFromScene += 
				new MEventManager.__Delegate_EntityRemovedFromScene(
				EventManager_EntityRemovedFromScene);
			pmEventMgr.EntityNameChanged +=
				new MEventManager.__Delegate_EntityNameChanged(
				EventManager_EntityNameChanged);
			pmEventMgr.EntityHiddenStateChanged += new MEventManager
				.__Delegate_EntityHiddenStateChanged(
				EventManager_EntityHiddenStateChanged);
			pmEventMgr.EntityFrozenStateChanged += new MEventManager
				.__Delegate_EntityFrozenStateChanged(
				EventManager_EntityFrozenStateChanged);
			pmEventMgr.SelectionSetAddedToScene += new MEventManager
				.__Delegate_SelectionSetAddedToScene(
				EventManager_SelectionSetAddedToScene);
			pmEventMgr.SelectionSetRemovedFromScene += new MEventManager
				.__Delegate_SelectionSetRemovedFromScene(
				EventManager_SelectionSetRemovedFromScene);
			pmEventMgr.SelectionSetNameChanged += new MEventManager
				.__Delegate_SelectionSetNameChanged(
				EventManager_SelectionSetNameChanged);
			pmEventMgr.NewSceneLoaded += new MEventManager
				.__Delegate_NewSceneLoaded(EventManager_NewSceneLoaded);
			pmEventMgr.LongOperationCompleted += new MEventManager
				.__Delegate_LongOperationCompleted(
				EventManager_LongOperationCompleted);
		}

		private void RegisterSettings()
		{
			SettingsService.RegisterSettingsObject(
				ms_strHiddenFilterSettingName, true,
				SettingsCategory.PerUser);
			SettingsService.RegisterSettingsObject(
				ms_strFrozenFilterSettingName, true,
				SettingsCategory.PerUser);
		}

		private void BuildAndSync()
		{
			BuildList();
			SyncListBoxToSelectionService();
		}

		private void BuildList()
		{
			MEntity[] amEntities =  MFramework.Instance.Scene.GetEntities();
			m_pmEntityList.Clear();
			foreach (MEntity pmEntity in amEntities)
			{
				bool bAdd = true;

				if ((m_eFilterState == FilterState.Hidden &&
						!pmEntity.Hidden) ||
					(m_eFilterState == FilterState.Frozen &&
						!pmEntity.Frozen) ||
					(m_eFilterState == FilterState.Visible &&
						(pmEntity.Hidden || pmEntity.Frozen)))
				{
					bAdd = false;
				}

				if (bAdd)
				{
					m_pmEntityList.Add(pmEntity);
				}
			}
			m_pmEntityList.Sort(m_pmEntityComparer);

			m_lbEntityList.BeginUpdate();
			int iTopIndex = m_lbEntityList.TopIndex;
			m_lbEntityList.Items.Clear();
			foreach (object pmObject in m_pmEntityList)
			{
				m_lbEntityList.Items.Add(pmObject);
			}
			m_lbEntityList.TopIndex = iTopIndex;
			m_lbEntityList.EndUpdate();
		}

		private void EnableSync()
		{
			m_bSynchingListBox = false;
			m_bSynchingSelectionService = false;
		}

		private void DisableSync()
		{
			m_bSynchingListBox = true;
			m_bSynchingSelectionService = true;
		}

		private void SyncListBoxToSelectionService()
		{
			if (!m_bSynchingSelectionService &&
				m_eFilterState == FilterState.Visible)
			{
				m_bSynchingListBox = true;
				m_bEnableListBoxSelectedIndexChanged = false;
				MEntity[] amSelectedEntities = SelectionService
					.GetSelectedEntities();
				int iInvalidIndex = amSelectedEntities.GetLowerBound(0) - 1;

				for (int i = 0; i < m_lbEntityList.Items.Count; i++)
				{
					MEntity pmEntity = m_lbEntityList.Items[i] as MEntity;
					Debug.Assert(pmEntity != null, "Invalid entity in list " +
						"box!");
					bool bIsSelected = (Array.IndexOf(amSelectedEntities,
						pmEntity) != iInvalidIndex);
					if (m_lbEntityList.GetSelected(i) != bIsSelected)
					{
						m_lbEntityList.SetSelected(i, bIsSelected);
					}
				}
				m_bEnableListBoxSelectedIndexChanged = true;
				m_bSynchingListBox = false;
			}
		}

		private void SyncSelectionServiceToListBox()
		{
			if (!m_bSynchingListBox &&
				m_eFilterState == FilterState.Visible)
			{
				m_amEntitiesSelectedAtLastSync = SelectionService
					.GetSelectedEntities();

				MEntity[] amEntitiesToAdd = BuildSelectionAdditionList();
				MEntity[] amEntitiesToRemove = BuildSelectionRemovalList();

				if (amEntitiesToRemove.Length > 0 ||
					amEntitiesToAdd.Length > 0)
				{
					int iNewNumSelectedEntities = SelectionService
						.NumSelectedEntities - amEntitiesToRemove.Length +
						amEntitiesToAdd.Length;
					CommandService.BeginUndoFrame("Replace main selection " +
						"with " + iNewNumSelectedEntities +
						(iNewNumSelectedEntities == 1 ? " entity" :
						" entities"));

					m_bSynchingSelectionService = true;

					SelectionService.RemoveEntitiesFromSelection(
						amEntitiesToRemove);
					SelectionService.AddEntitiesToSelection(amEntitiesToAdd);

					m_bSynchingSelectionService = false;

					CommandService.EndUndoFrame(SelectionService
						.CommandsAreUndoable);
				}
			}
		}

		private MEntity[] BuildSelectionRemovalList()
		{
			ArrayList pmReturnValue = new ArrayList();
			if (m_amEntitiesSelectedAtLastSync != null)
			{
				foreach (MEntity pmEntity in m_amEntitiesSelectedAtLastSync)
				{
					int iListBoxIndex = m_lbEntityList.Items.IndexOf(
						pmEntity);
					if (iListBoxIndex == -1)
					{
						SendNullEntityMessage(pmEntity.Name);
						break;
					}
					else
					{
						if (!m_lbEntityList.GetSelected(iListBoxIndex))
						{
							pmReturnValue.Add(pmEntity);
						}
					}
				}
			}
			return (MEntity[]) pmReturnValue.ToArray(typeof(MEntity));
		}

		private MEntity[] BuildSelectionAdditionList()
		{
			ArrayList pmReturnValue = new ArrayList();
			ListBox.SelectedIndexCollection pmIndices = m_lbEntityList
				.SelectedIndices;

			foreach (int iIndex in pmIndices)
			{
				MEntity pmEntity = m_lbEntityList.Items[iIndex] as MEntity;
				Debug.Assert(pmEntity != null, "Invalid entity in list box.");
				if (!MFramework.Instance.Scene.IsEntityInScene(pmEntity))
				{
					SendNullEntityMessage(pmEntity.Name);
					break;
				}
				else
				{
					bool bSelectedLastTime = false;
					if (m_amEntitiesSelectedAtLastSync != null)
					{
						int iIndexInLastSelection = Array.IndexOf(
							m_amEntitiesSelectedAtLastSync, pmEntity);
						bSelectedLastTime = iIndexInLastSelection > 
							(m_amEntitiesSelectedAtLastSync.GetLowerBound(0) 
							- 1);
					}
					if (!bSelectedLastTime)
					{
						pmReturnValue.Add(pmEntity);
					}
				}
			}
			return (MEntity[]) pmReturnValue.ToArray(typeof(MEntity));			
		}

		private static void SendNullEntityMessage(string strName)
		{
			PluginAPI.Message pmMessage = new PluginAPI.Message();
	
			pmMessage.m_strText = "Could not find entity: " + strName;
			pmMessage.m_strDetails = "The list box and the selection " +
				"manager appear to be out of sync.";
			MessageService.AddMessage(MessageChannelType.Errors, pmMessage);
		}

		private void SyncHideFreezeButtons()
		{
			bool bHideUnhideEnabled = false;
			string strHideUnhideToolTip = "Hide Entities";

			bool bFreezeUnfreezeEnabled = false;
			string strFreezeUnfreezeToolTip = "Freeze Entities";

			if (m_lbEntityList.SelectedItems.Count > 0)
			{
				switch (m_eFilterState)
				{
					case FilterState.Visible:
						bHideUnhideEnabled = true;
						bFreezeUnfreezeEnabled = true;
						break;
					case FilterState.Hidden:
						bHideUnhideEnabled = true;
						strHideUnhideToolTip = "Unhide Entities";
						break;
					case FilterState.Frozen:
						bFreezeUnfreezeEnabled = true;
						strFreezeUnfreezeToolTip = "Unfreeze Entities";
						break;
				}
			}

			m_tbbHideUnhide.Enabled = bHideUnhideEnabled;
			m_tbbHideUnhide.ToolTipText = strHideUnhideToolTip;

			m_tbbFreezeUnfreeze.Enabled = bFreezeUnfreezeEnabled;
			m_tbbFreezeUnfreeze.ToolTipText = strFreezeUnfreezeToolTip;
		}

		private void SetHiddenStateOnEntitiesInList(bool bHidden)
		{
			ArrayList pmEntitiesToChange = new ArrayList();
			foreach (MEntity pmEntity in m_lbEntityList.SelectedItems)
			{
				if (bHidden != pmEntity.Hidden)
				{
					pmEntitiesToChange.Add(pmEntity);
				}
			}

			if (pmEntitiesToChange.Count > 0)
			{
				string strCommand = bHidden ? "Hide " : "Unhide ";
				CommandService.BeginUndoFrame(strCommand +
					pmEntitiesToChange.Count + " entities");
				foreach (MEntity pmEntity in pmEntitiesToChange)
				{
					pmEntity.Hidden = bHidden;
				}
				CommandService.EndUndoFrame(true);
			}
		}

		private void SetFrozenStateOnEntitiesInList(bool bFrozen)
		{
			ArrayList pmEntitiesToChange = new ArrayList();
			foreach (MEntity pmEntity in m_lbEntityList.SelectedItems)
			{
				if (bFrozen != pmEntity.Frozen)
				{
					pmEntitiesToChange.Add(pmEntity);
				}
			}

			if (pmEntitiesToChange.Count > 0)
			{
				string strCommand = bFrozen ? "Freeze " : "Unfreeze ";
				CommandService.BeginUndoFrame(strCommand +
					pmEntitiesToChange.Count + " entities");
				foreach (MEntity pmEntity in pmEntitiesToChange)
				{
					pmEntity.Frozen = bFrozen;
				}
				CommandService.EndUndoFrame(true);
			}
		}

		private void SyncSelectionSetComboBox()
		{
			m_cbSelectionSets.Items.Clear();

			MSelectionSet[] amSelectionSets = SelectionSetService
				.GetSelectionSets();
			foreach (MSelectionSet pmSelectionSet in amSelectionSets)
			{
				m_cbSelectionSets.Items.Add(pmSelectionSet);
			}

			m_cbSelectionSets.SelectedItem = null;
		}

		private void ChangeFilterState(FilterState eFilterState)
		{
			bool bDisplayWarning;
			switch (eFilterState)
			{
				case FilterState.Hidden:
					bDisplayWarning = (bool) SettingsService
						.GetSettingsObject(ms_strHiddenFilterSettingName,
						SettingsCategory.PerUser);
					if (bDisplayWarning)
					{
						MessageBox.Show("The \"Hidden\" filter has just " +
							"been activated. Only hidden entities will\n" +
							"be displayed in the Selection Panel while " +
							"this filter is active.\nEntities selected in " +
							"the list box will not be selected in the " +
							"scene.\nRe-enable the \"Visible\" filter to " +
							"select entities in the scene.\n\nThis message " +
							"will only be displayed once.",
							"Hidden Filter Selected", MessageBoxButtons.OK,
							MessageBoxIcon.Information);

						SettingsService.SetSettingsObject(
							ms_strHiddenFilterSettingName, false,
							SettingsCategory.PerUser);
					}
					break;
				case FilterState.Frozen:
					bDisplayWarning = (bool) SettingsService
						.GetSettingsObject(ms_strFrozenFilterSettingName,
						SettingsCategory.PerUser);
					if (bDisplayWarning)
					{
						MessageBox.Show("The \"Frozen\" filter has just " +
							"been activated. Only frozen entities will\n" +
							"be displayed in the Selection Panel while " +
							"this filter is active.\nEntities selected in " +
							"the list box will not be selected in the " +
							"scene.\nRe-enable the \"Visible\" filter to " +
							"select entities in the scene.\n\nThis message " +
							"will only be displayed once.",
							"Frozen Filter Selected", MessageBoxButtons.OK,
							MessageBoxIcon.Information);

						SettingsService.SetSettingsObject(
							ms_strFrozenFilterSettingName, false,
							SettingsCategory.PerUser);
					}
					break;
			}

			m_eFilterState = eFilterState;
			BuildAndSync();
			SyncHideFreezeButtons();
			SyncFilterButton();
		}

		private void SyncFilterButton()
		{
			string strToolTip = "Change Filter:\n";
			switch (m_eFilterState)
			{
				case FilterState.Visible:
					m_tbbFilter.ToolTipText = strToolTip +
						"Visible Entities Shown";
					m_tbbFilter.ImageIndex = 5;
					break;
				case FilterState.Hidden:
					m_tbbFilter.ToolTipText = strToolTip +
						"Hidden Entities Shown";
					m_tbbFilter.ImageIndex = 6;
					break;
				case FilterState.Frozen:
					m_tbbFilter.ToolTipText = strToolTip +
						"Frozen Entities Shown";
					m_tbbFilter.ImageIndex = 7;
					break;
			}
		}

		private ContextMenu BuildContextMenu()
		{
			if (m_lbEntityList.SelectedItems.Count > 0)
			{
				ContextMenu menu = new ContextMenu();
				switch (m_eFilterState)
				{
					case FilterState.Visible:
					{
						MenuItem hideItem = menu.MenuItems.Add("Hide",
							new EventHandler(OnContextHide));
						MenuItem freezeItem = menu.MenuItems.Add("Freeze",
							new EventHandler(OnContextFreeze));
						MenuItem deleteItem = menu.MenuItems.Add("Delete",
							new EventHandler(OnContextDelete));
						break;
					}
					case FilterState.Frozen:
					{
						MenuItem unfreezeItem = menu.MenuItems.Add("UnFreeze",
							new EventHandler(OnContextUnfreeze));
						break;
					}
					case FilterState.Hidden:
					{
						MenuItem unhideItem = menu.MenuItems.Add("Unhide",
							new EventHandler(OnContextUnhide));
						break;
					}
				}
				if (m_lbEntityList.SelectedItems.Count == 1)
				{
					MenuItem separator = menu.MenuItems.Add("-");
					MenuItem lookAt = menu.MenuItems.Add("Look At",
						new EventHandler(OnContextLookAt));
					MenuItem zoomExtents = menu.MenuItems.Add("Zoom Extents",
						new EventHandler(OnContextZoomExtents));
					MenuItem findPalette = menu.MenuItems.Add("Show Template",
						new EventHandler(OnContextShowTemplate));
					MenuItem properties = menu.MenuItems.Add("Properties",
						new EventHandler(OnContextProperties));
					
				}

				return menu;
			}
			return null;
		}


		private void OnContextZoomExtents(object sender, EventArgs e)
		{
            if (MFramework.Instance.CameraManager.IsSceneCamera())//!/
                return;

			ServiceProvider sp = ServiceProvider.Instance;
			IUICommandService uiCommandService = 
				sp.GetService(typeof(IUICommandService)) as IUICommandService;

			UICommand zoomCommand =				
				uiCommandService.GetCommand("MoveToSelection");
			UIState state = new UIState();
			zoomCommand.ValidateCommand(state);
			if (state.Enabled)
			{
				zoomCommand.DoClick(this, null);
			}
		}

		private void OnContextLookAt(object sender, EventArgs e)
		{
			ServiceProvider sp = ServiceProvider.Instance;
			IUICommandService uiCommandService = 
				sp.GetService(typeof(IUICommandService)) as IUICommandService;

			UICommand lookAtCommand =				
				uiCommandService.GetCommand("LookAtSelection");
			UIState state = new UIState();
			lookAtCommand.ValidateCommand(state);
			if (state.Enabled)
			{
				lookAtCommand.DoClick(this, null);
			}
		}

		private void OnContextUnhide(object sender, EventArgs e)
		{
			SetHiddenStateOnEntitiesInList(false);
			SyncHideFreezeButtons();
		}

		private void OnContextUnfreeze(object sender, EventArgs e)
		{
			SetFrozenStateOnEntitiesInList(false);
			SyncHideFreezeButtons();
		}

		private void OnContextShowTemplate(object sender, EventArgs e)
		{
			MEntity entity = m_lbEntityList.SelectedItem as MEntity;
			if (entity != null)
			{
				if (entity.MasterEntity != null)
				{
					ServiceProvider sp = ServiceProvider.Instance;
					ICommandPanelService panelService = sp.GetService(
						typeof(ICommandPanelService)) as ICommandPanelService;
					panelService.ShowPanel("Template Properties", true);
					panelService.ShowPanel("Palettes", true);
					IEntityPathService pathService = sp.GetService(
						typeof(IEntityPathService)) as IEntityPathService;
					MPalette palette = 
						pathService.FindPaletteContainingEntity(
						entity.MasterEntity);
					MFramework.Instance.PaletteManager.ActivePalette =
						palette;
					palette.ActiveEntity = entity.MasterEntity;
				}
				else
				{
					MessageBox.Show(
						"Selected Entity does not inherit from a template.", 
						"Can't Show Template.", MessageBoxButtons.OK,
						MessageBoxIcon.Information);
				}
			}
		}

		private void OnContextFreeze(object sender, EventArgs e)
		{
			SetFrozenStateOnEntitiesInList(true);
			SyncHideFreezeButtons();
		}

		private void OnContextHide(object sender, EventArgs e)
		{
			SetHiddenStateOnEntitiesInList(true);			
			SyncHideFreezeButtons();
		}

		private void OnContextDelete(object sender, EventArgs e)
		{			
			IUICommandService uiCommandService = 
				ServiceProvider.Instance.GetService(
				typeof(IUICommandService)) as IUICommandService;
			UICommand command = 
				uiCommandService.GetCommand("DeleteSelectedEntities");
			UIState state = new UIState();
			command.ValidateCommand(state);
			if (state.Enabled)
			{
				command.DoClick(command, null);
			}

			SyncHideFreezeButtons();
		}

		private void OnContextProperties(object sender, EventArgs e)
		{
			ServiceProvider sp = ServiceProvider.Instance;
			ICommandPanelService panelService = sp.GetService(
				typeof(ICommandPanelService)) as ICommandPanelService;
			panelService.ShowPanel("Entity Properties", true);
		}


		#endregion

		#region WinForms handlers
		private void SelectionPanel_Load(object sender, System.EventArgs e)
		{
			AttachToEventManager();
			RegisterSettings();
			BuildList();
			SyncHideFreezeButtons();
			SyncFilterButton();
			SyncSelectionSetComboBox();
		}

		private void m_lbEntityList_MouseUp(object sender, 
			System.Windows.Forms.MouseEventArgs e)
		{
			if ((e.Button & MouseButtons.Right) == MouseButtons.Right)
			{
				ContextMenu menu = BuildContextMenu();
				if (menu != null)
				{
					menu.Show(m_lbEntityList, new Point(e.X, e.Y));
				}
			}
		}

		private void m_tbToolbar_ButtonClick(object sender,
			System.Windows.Forms.ToolBarButtonClickEventArgs e)
		{
			if (e.Button == m_tbbSelectAll)
			{
				m_tbbSelectAll_Click(sender, e);
			}
			else if (e.Button == m_tbbSelectNone)
			{
				m_tbbSelectNone_Click(sender, e);
			}
			else if (e.Button == m_tbbInvertSelection)
			{
				m_tbbInvertSelection_Click(sender, e);
			}
			else if (e.Button == m_tbbHideUnhide)
			{
				m_tbbHideUnhide_Click(sender, e);
			}
			else if (e.Button == m_tbbFreezeUnfreeze)
			{
				m_tbbFreezeUnfreeze_Click(sender, e);
			}
			else if (e.Button == m_tbbFilter)
			{
				bool bSetNext = false;
				foreach (FilterState eFilterState in Enum.GetValues(typeof(
					FilterState)))
				{
					if (eFilterState == m_eFilterState)
					{
						bSetNext = true;
					}
					else if (bSetNext)
					{
						ChangeFilterState(eFilterState);
						bSetNext = false;
						break;
					}
				}
				if (bSetNext)
				{
					// The current filter state is the last one in the enum.
					// Change it to the first filter state in the enum.
					Debug.Assert(Enum.GetValues(typeof(FilterState)).Length >
						0, "Invalid enumeration value array!");
					ChangeFilterState((FilterState) Enum.GetValues(typeof(
						FilterState)).GetValue(0));
				}
			}
			else if (e.Button == m_tbbCopyList)
			{
				string strText = "";
				// copy list to clipboard
				for (int i = 0; i < m_lbEntityList.Items.Count; i++)
				{
					if (m_lbEntityList.GetSelected(i))
					{
						strText += m_lbEntityList.Items[i].ToString();
						strText += "\r\n";
					}
				}
				if (strText != "")
				{
					Clipboard.Clear();
					Clipboard.SetText(strText);
				}
			}
			else if (e.Button == m_tbbSelectFromClip)
			{
				string strCliptext = Clipboard.GetText();
				SelectFromText(strCliptext, this.m_lbEntityList);
			}
		}

		private void m_tbToolbar_ButtonDropDown(object sender,
			System.Windows.Forms.ToolBarButtonClickEventArgs e)
		{
			if (e.Button == m_tbbFilter)
			{
				m_tbbFilter.DropDownMenu = new ContextMenu();
				foreach (string strFilterStateName in Enum.GetNames(typeof(
					FilterState)))
				{
					MenuItem pmMenuItem = new MenuItem(strFilterStateName,
						new EventHandler(m_tbbFilter_MenuItemClick));
					if (object.Equals(m_eFilterState.ToString(),
						pmMenuItem.Text))
					{
						pmMenuItem.Checked = true;
					}
					m_tbbFilter.DropDownMenu.MenuItems.Add(pmMenuItem);
				}
			}
		}

		private void m_tbbSelectAll_Click(object sender, System.EventArgs e)
		{   
			DisableSync();
			m_lbEntityList.BeginUpdate();
			m_bEnableListBoxSelectedIndexChanged = false;
			for (int i = 0; i < m_lbEntityList.Items.Count; i++)
			{
				m_lbEntityList.SetSelected(i, true);
			}
			m_bEnableListBoxSelectedIndexChanged = true;
			m_lbEntityList.EndUpdate();
			EnableSync();
			SyncSelectionServiceToListBox();
			SyncHideFreezeButtons();
		}

		private void m_tbbSelectNone_Click(object sender, System.EventArgs e)
		{
			DisableSync();
			m_lbEntityList.BeginUpdate();
			m_bEnableListBoxSelectedIndexChanged = false;
			for (int i = 0; i < m_lbEntityList.Items.Count; i++)
			{
				m_lbEntityList.SetSelected(i, false);
			}
			m_bEnableListBoxSelectedIndexChanged = true;
			m_lbEntityList.EndUpdate();		
			EnableSync();
			SyncSelectionServiceToListBox();
			SyncHideFreezeButtons();
		}

		private void m_tbbInvertSelection_Click(object sender,
			System.EventArgs e)
		{
			DisableSync();
			m_lbEntityList.BeginUpdate();
			m_bEnableListBoxSelectedIndexChanged = false;
			ListBox.SelectedIndexCollection pmIndices =
				m_lbEntityList.SelectedIndices;
			int iItemSize = m_lbEntityList.Items.Count;
			bool[] abNewIndices = new bool[iItemSize];

			for (int i = 0; i < iItemSize; i++)
			{
				abNewIndices[i] = !pmIndices.Contains(i);
			}

			for (int i = 0; i < m_lbEntityList.Items.Count; i++)
			{
				m_lbEntityList.SetSelected(i, abNewIndices[i]);
			}
			m_bEnableListBoxSelectedIndexChanged = true;
			m_lbEntityList.EndUpdate();	
			EnableSync();
			SyncSelectionServiceToListBox();
			SyncHideFreezeButtons();
		}

		private void m_lbEntityList_SelectedIndexChanged(
			object sender, System.EventArgs e)
		{
			if (m_bEnableListBoxSelectedIndexChanged)
			{
				SyncSelectionServiceToListBox();
				SyncHideFreezeButtons();
			}
		}

		private void SelectFromText(string strSelectText, ListBox lbEntities)
		{
			if (strSelectText.Length == 0)
			{
				lbEntities.SelectedIndex = -1;
				return;
			}

			string strTexts = strSelectText;
			if (strTexts.IndexOf("\r\n") != -1)
			{
				lbEntities.SelectedIndex = -1;
				string strRet = "\r\n";

				ArrayList pmTextList = new ArrayList();
				while (strTexts.IndexOf("\r\n") != -1)
				{
					int iCur = strTexts.IndexOf(strRet);
					string strSub = strTexts.Substring(0, iCur);
					pmTextList.Add(strSub);
					strTexts = strTexts.Remove(0, iCur + strRet.Length);
				}
				if (strTexts.Length > 0)
				{
					pmTextList.Add(strTexts);
				}

				int iCount = lbEntities.Items.Count;
				for (int i = 0; i < iCount; i++)
				{
					string strText1 = ((MEntity)lbEntities.Items[i]).Name;
					string strText1Low = strText1.ToLower();
					for (int j = 0; j < pmTextList.Count; j++)
					{
						string strText2 = (string)pmTextList[j];
						string strText2Low = strText2.ToLower();
						if (strText1Low == strText2Low)
						{
							lbEntities.SetSelected(i, true);
							j = pmTextList.Count;
						}
					}
				}
			}
			else
			{
				bool bFind = false;
				string strTextsLow = strTexts.ToLower();
				int iSelect = lbEntities.SelectedIndex;
				if (iSelect == -1) iSelect = 0;
				int iCount = lbEntities.Items.Count;
				for (int i = iSelect; i < iCount; i++)
				{
					string strText1 = ((MEntity)lbEntities.Items[i]).Name;
					string strText1Low = strText1.ToLower();
					if (strText1Low.IndexOf(strTextsLow) == 0)
					{
						lbEntities.SelectedIndex = -1;
						lbEntities.SetSelected(i, true);
						i = iCount;
						bFind = true;
					}
				}

				if (!bFind)
				{
					for (int i = 0; i < iCount; i++)
					{
						string strText1 = ((MEntity)lbEntities.Items[i]).Name;
						string strText1Low = strText1.ToLower();
						if (strText1Low.IndexOf(strTextsLow) == 0)
						{
							lbEntities.SelectedIndex = -1;
							lbEntities.SetSelected(i, true);
							i = iCount;
							bFind = true;
						}
					}
				}
			}
		}

		private void m_tbEntityName_TextChanged(object sender,
			System.EventArgs e)
		{
			if (m_bTextChangeCase == true)
			{
				m_bTextChangeCase = false;
				return;
			}
			string strText = m_tbEntityName.Text;
			SelectFromText(m_tbEntityName.Text, m_lbEntityList);
			int iRet = strText.IndexOf("\r\n");
			if (iRet != -1)
			{
				string strSub = strText.Substring(0, iRet);
				m_bTextChangeCase = true;
				m_tbEntityName.Text = strSub;
			}

			////////////////////////////////////////////////////////////////
			/*
			int iIndex = m_lbEntityList.FindString(m_tbEntityName.Text);
			if (iIndex != ListBox.NoMatches)
			{
				m_lbEntityList.TopIndex = iIndex;
			}
			*/
		}

		private void m_tbbHideUnhide_Click(object sender, System.EventArgs e)
		{
			SetHiddenStateOnEntitiesInList(m_eFilterState !=
				FilterState.Hidden);
			SyncHideFreezeButtons();
		}

		private void m_tbbFreezeUnfreeze_Click(object sender,
			System.EventArgs e)
		{
			SetFrozenStateOnEntitiesInList(m_eFilterState !=
				FilterState.Frozen);
			SyncHideFreezeButtons();
		}

		private void m_tbbFilter_MenuItemClick(object sender,
			System.EventArgs e)
		{
			MenuItem pmMenuItem = (MenuItem) sender;
			ChangeFilterState((FilterState) Enum.Parse(typeof(FilterState),
				pmMenuItem.Text));
		}

		private void m_lbEntityList_DrawItem(object sender,
			System.Windows.Forms.DrawItemEventArgs e)
		{
			if (e.Index > -1)
			{
				MEntity pmEntity = m_lbEntityList.Items[e.Index] as MEntity;
				Debug.Assert(pmEntity != null, "Invalid entity in list!");

				string strDrawString = pmEntity.Name;

				Font pmFont;
				if (pmEntity.Hidden)
				{
					pmFont = new Font(e.Font, FontStyle.Italic);
				}
				else
				{
					 pmFont = e.Font;
				}

				Brush pmBrush;
				if (pmEntity.Frozen)
				{
					pmBrush = Brushes.Blue;
				}
				else
				{
					pmBrush = new SolidBrush(e.ForeColor);
				}

				e.DrawBackground();
				e.DrawFocusRectangle();
				e.Graphics.DrawString(strDrawString, pmFont, pmBrush,
					e.Bounds);
			}
			else
			{
				e.DrawBackground();
				e.DrawFocusRectangle();
			}
		}

		private void m_lbEntityList_DoubleClick(object sender,
			System.EventArgs e)
		{
			switch (m_eFilterState)
			{
				case FilterState.Hidden:
					SetHiddenStateOnEntitiesInList(false);
					break;
				case FilterState.Frozen:
					SetFrozenStateOnEntitiesInList(false);
					break;
				case FilterState.Visible:
					OnContextZoomExtents(sender, e);
//					OnContextLookAt(sender, e);
					break;
			}
		}

		private void m_cbSelectionSets_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			if (m_cbSelectionSets.SelectedItem == null)
			{
				return;
			}

			MSelectionSet pmSelectionSet = m_cbSelectionSets.SelectedItem as MSelectionSet;
			Debug.Assert(pmSelectionSet != null, "Invalid selection " +
				"set in combo box!");
			SelectionService.ReplaceSelection(pmSelectionSet);

			//
			// HandOver, 강정욱 2008.01.31
			//
			// Visible이 아닌(Hide, Frozen) Select창에 있는 리스트중에
			// 물체들을 선택해 준다.
			if (m_eFilterState != FilterState.Visible)
			{
				MEntity[] entities = pmSelectionSet.GetEntities();
				foreach (MEntity entity in entities)
				{
					int iIdx = m_lbEntityList.FindString(entity.Name, 0);
					if (iIdx != -1)
					{
						m_lbEntityList.SelectedIndex = iIdx;
					}
				}
			}

			m_cbSelectionSets.SelectedItem = null;
		}

		private void OnKeyDown(object sender, 
			System.Windows.Forms.KeyEventArgs e)
		{
			if (m_tbEntityName.Focused)
			{
				return;
			}
			switch (e.KeyData)
			{
				case Keys.Delete:
				{
					e.Handled = true;
					IUICommandService uiCommandService = 
						ServiceProvider.Instance.GetService(
						typeof(IUICommandService)) as IUICommandService;
					UICommand command = 
						uiCommandService.GetCommand("DeleteSelectedEntities");
					UIState state = new UIState();
					command.ValidateCommand(state);
					if (state.Enabled)
					{
						command.DoClick(command, null);
					}
					break;
				}
			}
		
		}

		#endregion

		#region UICommand Handlers
		[UICommandHandler("SelectAllEntities")]
		private void OnSelectAllEntities(object sender, 
			EventArgs args)
		{
			MEntity[] sceneEntities = MFramework.Instance.Scene.GetEntities();
			//Note that we dont not have to filter out the frozen and hidden
			//Entities, as the selections service now does that for us.
			SelectionService.ReplaceSelection(sceneEntities);		  
		}

		[UICommandHandler("DeselectAllEntities")]
		private void OnDeselectAllEntities(object sender, 
			EventArgs args)
		{
			SelectionService.ClearSelectedEntities();
		}


		[UICommandHandler("CloneSelection")]
		private void OnCloneSelected(object sender, EventArgs args)
		{			
			ArrayList buffer = new ArrayList();
			foreach(MEntity entity in SelectionService.GetSelectedEntities())
			{
				buffer.Add(entity);
			}
			//m_copyPasteBuffer = buffer.ToArray(typeof(MEntity)) as MEntity[];
			CommandService.BeginUndoFrame(string.Format(
			"Cloned {0} Entities", buffer.Count));
			ArrayList entitiesToAdd = new ArrayList();
			foreach (MEntity entity in buffer)
			{
				string newName = MFramework.Instance.Scene.GetUniqueEntityName(entity.Name);
				MEntity newEntity = entity.Clone(newName, false);
				entitiesToAdd.Add(newEntity);
				MFramework.Instance.Scene.AddEntity(newEntity, true);
			}
			SelectionService.ReplaceSelection(entitiesToAdd.ToArray(typeof(MEntity)) as MEntity[]);
			CommandService.EndUndoFrame(true);
		}

		[UICommandValidator("CloneSelection")]
		private void OnValidateCloneSelection(object sender, UIState state)
		{
			state.Enabled = SelectionService.GetSelectedEntities().Length > 0;
		}



		#endregion



		#region EntityComparer class
		private class EntityComparer : IComparer
		{
			#region IComparer Members
			public int Compare(object pmObjectA, object pmObjectB)
			{
				MEntity pmEntityA = pmObjectA as MEntity;
				MEntity pmEntityB = pmObjectB as MEntity;
				if (pmEntityA == null || pmEntityB == null)
				{
					throw new ArgumentException("EntityComparer only " +
						"supports comparing MEntity instances.");
				}

				return pmEntityA.Name.CompareTo(pmEntityB.Name);
			}
			#endregion
		}
		#endregion
	}
}

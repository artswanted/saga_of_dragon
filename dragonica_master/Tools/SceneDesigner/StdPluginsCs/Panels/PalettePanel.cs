// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2006 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

using System;
using System.Collections;
using System.Drawing;
using System.IO;
using System.Windows.Forms;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;
using Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Dialogs;
using Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Utility;
using Message = Emergent.Gamebryo.SceneDesigner.PluginAPI.Message;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Panels
{
	public class PalettePanel : Form
	{


        #region Private Data
        internal static readonly Color m_cOrphanColor = Color.Blue;
        //string m_currentPath;
        MPalette m_currentPalette;
        string m_currentCategory;
        MEntity m_currentEntity;
        ArrayList m_comboDrawHints;
        ISelectionService m_selectionService;
        ISettingsService m_settingsService;
        IUICommandService m_uiCommandService;
        ICommandPanelService m_commandPanelService;
        private IEntityPathService m_entityPathService;
        private ICommandService m_commandService;

        //bool m_bIsSyncingUI;
        bool m_bIsSyncingDropDown;
        bool m_bIsSyncingListView;
        bool m_bIsSyncingPaletteManagerToDropDown;
        bool m_bIsSyncingPaletteManagerToListView;
        private const string m_listSettingName = 
            "PalettePanel.LargeIconView";
        private const string m_saveIndicatorText = " (modified)";

        //internal static readonly Color m_cDirtyColor = Color.Red;
        #endregion

        private System.Windows.Forms.ComboBox m_cbPalettePath;
        private System.Windows.Forms.ListView m_lvPalettes;
        private System.Windows.Forms.ImageList m_ilViewIcons;
        private System.Windows.Forms.ImageList m_ilViewIconsSmall;
        private System.Windows.Forms.ToolBarButton m_tbbImportPalette;
        private System.Windows.Forms.ToolBarButton m_tbbNewPalette;
        private System.Windows.Forms.ToolBarButton m_tbbNewTemplate;
        private System.Windows.Forms.ToolBarButton m_tbbCloneFromEntity;
        private System.Windows.Forms.ToolBarButton m_tbbMoveCopyRename;
        private System.Windows.Forms.ToolBarButton m_tbbDeleteTemplate;
        private System.Windows.Forms.ToolBarButton m_tbbAddArtAssets;
        private System.Windows.Forms.ToolBarButton m_tbbSeparator2;
        private System.Windows.Forms.ToolBarButton m_tbbLargeIcons;
        private System.Windows.Forms.ToolBarButton m_tbbSmallIcons;
        private System.Windows.Forms.ToolBar m_tbToolBar;
        private System.Windows.Forms.ContextMenu m_contextMenu;
        private System.Windows.Forms.MenuItem m_menuMoveRename;
        private System.Windows.Forms.MenuItem m_menuCopy;
        private System.Windows.Forms.MenuItem m_menuDelete;
        private System.Windows.Forms.Panel m_pnlPlaceHolder;
        private System.Windows.Forms.Timer m_timer;
        //private System.Windows.Forms.CheckBox checkBox1;
		private System.ComponentModel.IContainer components = null;
        private System.Windows.Forms.ToolBarButton m_tbbRenamePalette;
        private System.Windows.Forms.ToolBarButton m_tbbDeletePalette;
        private System.Windows.Forms.ToolBarButton m_tbbSeparator1;
        private System.Windows.Forms.ImageList m_ilToolbarImages;
        private System.Windows.Forms.ToolTip m_ttToolTip;
        private System.Windows.Forms.MenuItem m_menuSelectInScene;
        private System.Windows.Forms.MenuItem m_menuProperties;
	    private int m_iLastDirtyPaletteCount = int.MaxValue;

	    public PalettePanel()
		{
			// This call is required by the Windows Form Designer.
            
			InitializeComponent();
            m_currentPalette = null;
            m_currentCategory = "";

            SettingsService.RegisterSettingsObject(
                m_listSettingName, true, SettingsCategory.PerUser);

            UICommandService.BindCommands(this);

            FW.EventManager.PaletteAdded += 
                new MEventManager.__Delegate_PaletteAdded(
                EventManager_PaletteAdded);

            FW.EventManager.PaletteRemoved += 
                new MEventManager.__Delegate_PaletteRemoved(
                EventManager_PaletteRemoved);

            FW.EventManager.ActivePaletteChanged += 
                new MEventManager.__Delegate_ActivePaletteChanged(
                EventManager_ActivePaletteChanged);

            FW.EventManager.PaletteActiveEntityChanged += 
                new MEventManager.__Delegate_PaletteActiveEntityChanged(
                EventManager_PaletteActiveEntityChanged);

            FW.EventManager.EntityAddedToScene +=
                new MEventManager.__Delegate_EntityAddedToScene(
                EventManager_EntityAddedToPalette);

            FW.EventManager.EntityRemovedFromScene += 
                new MEventManager.__Delegate_EntityRemovedFromScene(
                EventManager_EntityRemovedFromPalette);


//            FW.EventManager.EntityAddedToPalette += 
//                new MEventManager.__Delegate_EntityAddedToPalette(
//                EventManager_EntityAddedToPalette);
//
//            FW.EventManager.EntityRemovedFromPalette += 
//                new MEventManager.__Delegate_EntityRemovedFromPalette(
//                EventManager_EntityRemovedFromPalette);
		}


        private enum ItemType
        {
            Palette,
            Category,
            Template
        }

        private class ComboDrawHint
        {
            public ItemType Type;
            public int indent;
            public string CategoryName;
            public MPalette Palette;
        }

        private MFramework FW
        {
            get
            { return MFramework.Instance; }
        }

        private ISelectionService SelectionService
        {
            get
            {
                if (m_selectionService == null)
                {
                    ServiceProvider sp = ServiceProvider.Instance;
                    m_selectionService = 
                        sp.GetService(typeof(ISelectionService))
                        as ISelectionService;
                }
                return m_selectionService;
            }
        }

        private ISettingsService SettingsService
        {
            get
            {
                if (m_settingsService == null)
                {
                    ServiceProvider sp = ServiceProvider.Instance;
                    m_settingsService = 
                        sp.GetService(typeof(ISettingsService)) 
                        as ISettingsService;                    
                }
                return m_settingsService;
            }
        }

        private IUICommandService UICommandService
        {
            get
            {
                if (m_uiCommandService == null)
                {
                    ServiceProvider sp = ServiceProvider.Instance;
                    m_uiCommandService = 
                        sp.GetService(typeof(IUICommandService)) 
                        as IUICommandService;                    
                }
                return m_uiCommandService;
            }
        }

        private ICommandPanelService CommandPanelService
        {
            get
            {
                if (m_commandPanelService == null)
                {
                    ServiceProvider sp = ServiceProvider.Instance;

                    m_commandPanelService = 
                        sp.GetService(typeof(ICommandPanelService)) 
                        as ICommandPanelService;                    
                }
                return m_commandPanelService;
            }
        }

        private ICommandService CommandService
        {
            get
            {
                if (m_commandService == null)
                {
                    ServiceProvider sp = ServiceProvider.Instance;

                    m_commandService = 
                        sp.GetService(typeof(ICommandService)) 
                        as ICommandService;                                        
                }
                return m_commandService;
            }
        }

        private IEntityPathService EntityPathService
        {
            get
            {
                if (m_entityPathService == null)
                {
                    ServiceProvider sp = ServiceProvider.Instance;

                    m_entityPathService = 
                        sp.GetService(typeof(IEntityPathService)) 
                        as IEntityPathService;                                        
                }
                return m_entityPathService;
            }
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(PalettePanel));
            this.m_cbPalettePath = new System.Windows.Forms.ComboBox();
            this.m_lvPalettes = new System.Windows.Forms.ListView();
            this.m_ilViewIcons = new System.Windows.Forms.ImageList(this.components);
            this.m_ilViewIconsSmall = new System.Windows.Forms.ImageList(this.components);
            this.m_tbToolBar = new System.Windows.Forms.ToolBar();
            this.m_tbbImportPalette = new System.Windows.Forms.ToolBarButton();
            this.m_tbbNewPalette = new System.Windows.Forms.ToolBarButton();
            this.m_tbbRenamePalette = new System.Windows.Forms.ToolBarButton();
            this.m_tbbDeletePalette = new System.Windows.Forms.ToolBarButton();
            this.m_tbbSeparator1 = new System.Windows.Forms.ToolBarButton();
            this.m_tbbNewTemplate = new System.Windows.Forms.ToolBarButton();
            this.m_tbbCloneFromEntity = new System.Windows.Forms.ToolBarButton();
            this.m_tbbMoveCopyRename = new System.Windows.Forms.ToolBarButton();
            this.m_tbbDeleteTemplate = new System.Windows.Forms.ToolBarButton();
            this.m_tbbAddArtAssets = new System.Windows.Forms.ToolBarButton();
            this.m_tbbSeparator2 = new System.Windows.Forms.ToolBarButton();
            this.m_tbbLargeIcons = new System.Windows.Forms.ToolBarButton();
            this.m_tbbSmallIcons = new System.Windows.Forms.ToolBarButton();
            this.m_ilToolbarImages = new System.Windows.Forms.ImageList(this.components);
            this.m_contextMenu = new System.Windows.Forms.ContextMenu();
            this.m_menuMoveRename = new System.Windows.Forms.MenuItem();
            this.m_menuCopy = new System.Windows.Forms.MenuItem();
            this.m_menuDelete = new System.Windows.Forms.MenuItem();
            this.m_menuSelectInScene = new System.Windows.Forms.MenuItem();
            this.m_menuProperties = new System.Windows.Forms.MenuItem();
            this.m_pnlPlaceHolder = new System.Windows.Forms.Panel();
            this.m_timer = new System.Windows.Forms.Timer(this.components);
            this.m_ttToolTip = new System.Windows.Forms.ToolTip(this.components);
            this.m_pnlPlaceHolder.SuspendLayout();
            this.SuspendLayout();
            // 
            // m_cbPalettePath
            // 
            this.m_cbPalettePath.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_cbPalettePath.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.m_cbPalettePath.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.m_cbPalettePath.Location = new System.Drawing.Point(10, 0);
            this.m_cbPalettePath.Name = "m_cbPalettePath";
            this.m_cbPalettePath.Size = new System.Drawing.Size(597, 22);
            this.m_cbPalettePath.TabIndex = 1;
            this.m_cbPalettePath.DrawItem += new System.Windows.Forms.DrawItemEventHandler(this.m_cbPalettePath_DrawItem);
            this.m_cbPalettePath.SelectionChangeCommitted += new System.EventHandler(this.m_cbPalettePath_SelectionChangeCommitted);
            // 
            // m_lvPalettes
            // 
            this.m_lvPalettes.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_lvPalettes.HideSelection = false;
            this.m_lvPalettes.LargeImageList = this.m_ilViewIcons;
            this.m_lvPalettes.Location = new System.Drawing.Point(10, 26);
            this.m_lvPalettes.MultiSelect = false;
            this.m_lvPalettes.Name = "m_lvPalettes";
            this.m_lvPalettes.Size = new System.Drawing.Size(597, 333);
            this.m_lvPalettes.SmallImageList = this.m_ilViewIconsSmall;
            this.m_lvPalettes.Sorting = System.Windows.Forms.SortOrder.Ascending;
            this.m_lvPalettes.TabIndex = 0;
            this.m_lvPalettes.UseCompatibleStateImageBehavior = false;
            this.m_lvPalettes.ItemActivate += new System.EventHandler(this.m_lvPalettes_ItemActivate);
            this.m_lvPalettes.SelectedIndexChanged += new System.EventHandler(this.m_lvPalettes_SelectedIndexChanged);
            this.m_lvPalettes.MouseUp += new System.Windows.Forms.MouseEventHandler(this.m_lvPalettes_MouseUp);
            this.m_lvPalettes.KeyDown += new System.Windows.Forms.KeyEventHandler(this.m_lvPalettes_KeyDown);
            // 
            // m_ilViewIcons
            // 
            this.m_ilViewIcons.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("m_ilViewIcons.ImageStream")));
            this.m_ilViewIcons.TransparentColor = System.Drawing.Color.Transparent;
            this.m_ilViewIcons.Images.SetKeyName(0, "");
            this.m_ilViewIcons.Images.SetKeyName(1, "");
            this.m_ilViewIcons.Images.SetKeyName(2, "");
            // 
            // m_ilViewIconsSmall
            // 
            this.m_ilViewIconsSmall.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("m_ilViewIconsSmall.ImageStream")));
            this.m_ilViewIconsSmall.TransparentColor = System.Drawing.Color.Transparent;
            this.m_ilViewIconsSmall.Images.SetKeyName(0, "");
            this.m_ilViewIconsSmall.Images.SetKeyName(1, "");
            this.m_ilViewIconsSmall.Images.SetKeyName(2, "");
            // 
            // m_tbToolBar
            // 
            this.m_tbToolBar.Appearance = System.Windows.Forms.ToolBarAppearance.Flat;
            this.m_tbToolBar.Buttons.AddRange(new System.Windows.Forms.ToolBarButton[] {
            this.m_tbbImportPalette,
            this.m_tbbNewPalette,
            this.m_tbbRenamePalette,
            this.m_tbbDeletePalette,
            this.m_tbbSeparator1,
            this.m_tbbNewTemplate,
            this.m_tbbCloneFromEntity,
            this.m_tbbMoveCopyRename,
            this.m_tbbDeleteTemplate,
            this.m_tbbAddArtAssets,
            this.m_tbbSeparator2,
            this.m_tbbLargeIcons,
            this.m_tbbSmallIcons});
            this.m_tbToolBar.Divider = false;
            this.m_tbToolBar.DropDownArrows = true;
            this.m_tbToolBar.ImageList = this.m_ilToolbarImages;
            this.m_tbToolBar.Location = new System.Drawing.Point(0, 0);
            this.m_tbToolBar.Name = "m_tbToolBar";
            this.m_tbToolBar.ShowToolTips = true;
            this.m_tbToolBar.Size = new System.Drawing.Size(616, 26);
            this.m_tbToolBar.TabIndex = 0;
            this.m_tbToolBar.ButtonClick += new System.Windows.Forms.ToolBarButtonClickEventHandler(this.m_tbToolBar_ButtonClick);
            // 
            // m_tbbImportPalette
            // 
            this.m_tbbImportPalette.ImageIndex = 0;
            this.m_tbbImportPalette.Name = "m_tbbImportPalette";
            this.m_tbbImportPalette.ToolTipText = "Import Palette";
            // 
            // m_tbbNewPalette
            // 
            this.m_tbbNewPalette.ImageIndex = 1;
            this.m_tbbNewPalette.Name = "m_tbbNewPalette";
            this.m_tbbNewPalette.ToolTipText = "New Palette";
            // 
            // m_tbbRenamePalette
            // 
            this.m_tbbRenamePalette.ImageIndex = 2;
            this.m_tbbRenamePalette.Name = "m_tbbRenamePalette";
            this.m_tbbRenamePalette.ToolTipText = "Rename Palette";
            // 
            // m_tbbDeletePalette
            // 
            this.m_tbbDeletePalette.ImageIndex = 3;
            this.m_tbbDeletePalette.Name = "m_tbbDeletePalette";
            this.m_tbbDeletePalette.ToolTipText = "Delete Palette";
            // 
            // m_tbbSeparator1
            // 
            this.m_tbbSeparator1.Name = "m_tbbSeparator1";
            this.m_tbbSeparator1.Style = System.Windows.Forms.ToolBarButtonStyle.Separator;
            // 
            // m_tbbNewTemplate
            // 
            this.m_tbbNewTemplate.ImageIndex = 4;
            this.m_tbbNewTemplate.Name = "m_tbbNewTemplate";
            this.m_tbbNewTemplate.ToolTipText = "New Template";
            // 
            // m_tbbCloneFromEntity
            // 
            this.m_tbbCloneFromEntity.ImageIndex = 5;
            this.m_tbbCloneFromEntity.Name = "m_tbbCloneFromEntity";
            this.m_tbbCloneFromEntity.ToolTipText = "Clone Scene Entity As New Template";
            // 
            // m_tbbMoveCopyRename
            // 
            this.m_tbbMoveCopyRename.ImageIndex = 6;
            this.m_tbbMoveCopyRename.Name = "m_tbbMoveCopyRename";
            this.m_tbbMoveCopyRename.ToolTipText = "Move, Copy, or Rename Template";
            // 
            // m_tbbDeleteTemplate
            // 
            this.m_tbbDeleteTemplate.ImageIndex = 7;
            this.m_tbbDeleteTemplate.Name = "m_tbbDeleteTemplate";
            this.m_tbbDeleteTemplate.ToolTipText = "Delete Template";
            // 
            // m_tbbAddArtAssets
            // 
            this.m_tbbAddArtAssets.ImageIndex = 8;
            this.m_tbbAddArtAssets.Name = "m_tbbAddArtAssets";
            this.m_tbbAddArtAssets.ToolTipText = "Add Art Assets As Templates";
            // 
            // m_tbbSeparator2
            // 
            this.m_tbbSeparator2.Name = "m_tbbSeparator2";
            this.m_tbbSeparator2.Style = System.Windows.Forms.ToolBarButtonStyle.Separator;
            // 
            // m_tbbLargeIcons
            // 
            this.m_tbbLargeIcons.ImageIndex = 9;
            this.m_tbbLargeIcons.Name = "m_tbbLargeIcons";
            this.m_tbbLargeIcons.Style = System.Windows.Forms.ToolBarButtonStyle.ToggleButton;
            this.m_tbbLargeIcons.ToolTipText = "Large Icons";
            // 
            // m_tbbSmallIcons
            // 
            this.m_tbbSmallIcons.ImageIndex = 10;
            this.m_tbbSmallIcons.Name = "m_tbbSmallIcons";
            this.m_tbbSmallIcons.Style = System.Windows.Forms.ToolBarButtonStyle.ToggleButton;
            this.m_tbbSmallIcons.ToolTipText = "Small Icons";
            // 
            // m_ilToolbarImages
            // 
            this.m_ilToolbarImages.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("m_ilToolbarImages.ImageStream")));
            this.m_ilToolbarImages.TransparentColor = System.Drawing.Color.Fuchsia;
            this.m_ilToolbarImages.Images.SetKeyName(0, "");
            this.m_ilToolbarImages.Images.SetKeyName(1, "");
            this.m_ilToolbarImages.Images.SetKeyName(2, "");
            this.m_ilToolbarImages.Images.SetKeyName(3, "");
            this.m_ilToolbarImages.Images.SetKeyName(4, "");
            this.m_ilToolbarImages.Images.SetKeyName(5, "");
            this.m_ilToolbarImages.Images.SetKeyName(6, "");
            this.m_ilToolbarImages.Images.SetKeyName(7, "");
            this.m_ilToolbarImages.Images.SetKeyName(8, "");
            this.m_ilToolbarImages.Images.SetKeyName(9, "");
            this.m_ilToolbarImages.Images.SetKeyName(10, "");
            // 
            // m_contextMenu
            // 
            this.m_contextMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
            this.m_menuMoveRename,
            this.m_menuCopy,
            this.m_menuDelete,
            this.m_menuSelectInScene,
            this.m_menuProperties});
            // 
            // m_menuMoveRename
            // 
            this.m_menuMoveRename.Index = 0;
            this.m_menuMoveRename.Text = "&Move/Rename...";
            this.m_menuMoveRename.Click += new System.EventHandler(this.m_menuMoveRename_Click);
            // 
            // m_menuCopy
            // 
            this.m_menuCopy.Index = 1;
            this.m_menuCopy.Text = "&Copy...";
            this.m_menuCopy.Click += new System.EventHandler(this.m_menuCopy_Click);
            // 
            // m_menuDelete
            // 
            this.m_menuDelete.Index = 2;
            this.m_menuDelete.Text = "&Delete...";
            this.m_menuDelete.Click += new System.EventHandler(this.m_menuDelete_Click);
            // 
            // m_menuSelectInScene
            // 
            this.m_menuSelectInScene.Index = 3;
            this.m_menuSelectInScene.Text = "Select In Scene";
            this.m_menuSelectInScene.Click += new System.EventHandler(this.m_menuSelectInScene_Click);
            // 
            // m_menuProperties
            // 
            this.m_menuProperties.Index = 4;
            this.m_menuProperties.Text = "Properties";
            this.m_menuProperties.Click += new System.EventHandler(this.m_menuProperties_Click);
            // 
            // m_pnlPlaceHolder
            // 
            this.m_pnlPlaceHolder.BackColor = System.Drawing.SystemColors.Control;
            this.m_pnlPlaceHolder.Controls.Add(this.m_lvPalettes);
            this.m_pnlPlaceHolder.Controls.Add(this.m_cbPalettePath);
            this.m_pnlPlaceHolder.Dock = System.Windows.Forms.DockStyle.Fill;
            this.m_pnlPlaceHolder.Location = new System.Drawing.Point(0, 26);
            this.m_pnlPlaceHolder.Name = "m_pnlPlaceHolder";
            this.m_pnlPlaceHolder.Size = new System.Drawing.Size(616, 372);
            this.m_pnlPlaceHolder.TabIndex = 1;
            // 
            // m_timer
            // 
            this.m_timer.Interval = 1000;
            this.m_timer.Tick += new System.EventHandler(this.m_timer_Tick);
            // 
            // PalettePanel
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(6, 14);
            this.AutoScroll = true;
            this.ClientSize = new System.Drawing.Size(616, 398);
            this.Controls.Add(this.m_pnlPlaceHolder);
            this.Controls.Add(this.m_tbToolBar);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "PalettePanel";
            this.Text = "Palettes";
            this.Load += new System.EventHandler(this.PalettePanel_Load);
            this.m_pnlPlaceHolder.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }
		#endregion

        #region Winforms handlers

        private void PalettePanel_Load(object sender, System.EventArgs e)
        {
            FW.PaletteManager.ActivePalette = null;
            SyncUI();
            bool largeIconView = (bool) SettingsService.GetSettingsObject(
                m_listSettingName, SettingsCategory.PerUser);
            if (largeIconView)
            {
                m_btnLargeIconView_Click(this, e);
            }
            else
            {
                m_btnSmallIconView_Click(this, e);
            }
            m_timer.Start();
        }

        private void m_btnImportPalette_Click(object sender, 
            System.EventArgs e)
        {
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Filter = "Palette files (*.pal) |*.pal";
            dlg.CheckFileExists = true;
            //We cannot import multiple files at once, because 
            //import order can be important.
            dlg.Multiselect = true;
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                IMessageService messageService = 
                    ServiceProvider.Instance.GetService(
                    typeof(IMessageService)) as IMessageService;
                messageService.ClearMessages(MessageChannelType.Conflicts);
                PaletteUtilities.ImportPalettes(dlg.FileNames);
                MFramework.Instance.Scene.Dirty = true;
            }
        }

        private void m_lvPalettes_ItemActivate(object sender, 
            System.EventArgs e)
        {
            if (!m_bIsSyncingListView)
            {
                ListView.SelectedListViewItemCollection selectedItems = 
                    m_lvPalettes.SelectedItems;
                if (selectedItems.Count > 0)
                {
                    ListViewItem item = selectedItems[0];
                    ItemType itemType = (ItemType) item.Tag;
                    switch(itemType)
                    {
                        case ItemType.Palette:
                        {
                            m_currentPalette = 
                                FW.PaletteManager.GetPaletteByName(item.Text);
                            m_currentEntity = null;
                            break;
                        }
                        case ItemType.Category:
                        {
                            string strItemText = item.Text.Substring(1);

                            if (strItemText.Equals(".."))
                            {
                                if (m_currentCategory.Equals(string.Empty))
                                {
                                    m_currentPalette = null;
                                }
                                else
                                {
                                    m_currentCategory = 
                                        PaletteUtilities.ParentCategory(
                                        m_currentCategory);
                                }
                            }
                            else
                            {
                                if (m_currentCategory.Equals(string.Empty))
                                {
                                    m_currentCategory = strItemText;
                                }
                                else
                                {
                                    m_currentCategory = 
                                        string.Format("{0}.{1}",
                                        m_currentCategory, strItemText);                                   
                                }                                
                            }
                            m_currentEntity = null;
                            break;
                        }
                        case ItemType.Template:
                        {
                            UICommand command = UICommandService.GetCommand(
                                "CreateInteractionMode");
                            if (command != null)
                            {
                                command.DoClick(this, null);
                            }
                            break;
                        }
                        default:
                        {
                            break;
                        }
                    }
                }
                SyncPaletteManagerToListView();   
                SyncUI();
            }
        }

        private void m_lvPalettes_SelectedIndexChanged(object sender, 
            System.EventArgs e)
        {
            if (!m_bIsSyncingListView)
            {
                ListView.SelectedListViewItemCollection selectedItems = 
                    m_lvPalettes.SelectedItems;
                if (selectedItems.Count > 0)
                {
                    ListViewItem item = selectedItems[0];
                    ItemType itemType = (ItemType) item.Tag;
                    switch (itemType)
                    {
                        case ItemType.Template:
                        {
                            m_currentEntity = 
                                m_currentPalette.GetEntityByName(
                                AppendPaletteNameAndCategory(item.Text, 
                                m_currentPalette, m_currentCategory));   
                            if (m_currentEntity == null)
                            {
                                //try to find it without the palette name
                                if (m_currentCategory == string.Empty)
                                {
                                    m_currentEntity = 
                                        m_currentPalette.GetEntityByName(
                                        item.Text);
                                }
                                else
                                {
                                    m_currentEntity = 
                                        m_currentPalette.GetEntityByName(
                                        m_currentCategory + "." + item.Text);                                    
                                }
                            }
                            break;
                        }
                        default:
                        {
                            if (m_currentPalette != null)
                            {
                                m_currentEntity = null;
                            }
                            break;
                        }
                    }
                }
                else
                {
                    m_currentEntity = null;
                }
                //We shouldn't need to sync the dropdown, 
                //since changing the selection shouldn't effect what's in the combo
                //We still need to sync the Palette manager
                SyncPaletteManagerToListView();
            }
        }

	    private string AppendPaletteNameAndCategory(string entityName, 
            MPalette palette, string category)
	    {
            string retVal = entityName;
	        if (!category.Equals(string.Empty))
	        {
	            retVal = string.Format("{0}.{1}", category,  retVal);
	        }
	        retVal = string.Format("[{0}]{1}", palette.Name, retVal);
            return retVal;
	        

	    }

	    private void m_cbPalettePath_DrawItem(object sender, 
            System.Windows.Forms.DrawItemEventArgs e)
        {
            e.DrawBackground();
            e.DrawFocusRectangle();
            Font drawFont = e.Font;
            Color textColor = e.ForeColor;
            if (e.Index > -1)
            {
                ComboDrawHint hint = m_comboDrawHints[e.Index] 
                    as ComboDrawHint;
                if ( hint.Palette != null)
                {
                    if (hint.Palette.DontSave)
                    {
                        textColor = m_cOrphanColor;
                    }
                }
                if((e.State & DrawItemState.ComboBoxEdit) 
                    == DrawItemState.ComboBoxEdit)
                {
                    string text = "Null String";
                    //string saveIndicator = string.Empty;
                    if (hint.Type == ItemType.Palette || 
                        hint.Type == ItemType.Category)
                    {
                        //if
                    }
                    switch (hint.Type)
                    {
                        case ItemType.Palette:
                        {
                            string saveIndicator = string.Empty;
                            if (//!hint.Palette.DontSave && 
                                hint.Palette != null &&
                                hint.Palette.Scene.Dirty)
                            {
                                drawFont = new Font(drawFont, FontStyle.Bold);
                                saveIndicator = m_saveIndicatorText;
                            }
                            text = string.Format(" [{0}]{1}",
                                m_cbPalettePath.Items[e.Index].ToString(),
                                saveIndicator);
                            break;
                        }
                        case ItemType.Category:
                        {
                            string saveIndicator = string.Empty;
                            if (//!m_currentPalette.DontSave && 
                                m_currentPalette.Scene.Dirty)
                            {
                                drawFont = new Font(drawFont, FontStyle.Bold);
                                saveIndicator = m_saveIndicatorText;
                            }
                            text = string.Format(" [{0}].{1}{2}",
                                m_currentPalette.Name,
                                m_currentCategory, saveIndicator);
                            break;
                        }
                        default:
                        {
                            text = "Error, bad item type!";
                            break;
                        }
                    }
                    e.Graphics.DrawString(
                        text, drawFont, new SolidBrush(textColor), 
                        0,
                        e.Bounds.Top);
                }
                else
                {

                    Image image = m_ilViewIconsSmall.Images[(int) hint.Type];                
                    Size imageSize = m_ilViewIconsSmall.ImageSize;

                    int indent = e.Bounds.Left + hint.indent;

                    string saveIndicator = string.Empty;
                    switch (hint.Type)
                    {
                        case ItemType.Palette:
                        {
                            if (hint.Palette != null &&
                                hint.Palette.Scene.Dirty)
                            {
                                saveIndicator = m_saveIndicatorText;
                                drawFont = new Font(drawFont, FontStyle.Bold);
                            }
                            break;
                        }
                        default:
                        {
                            saveIndicator = string.Empty;
                            break;
                        }
                    }
                    if (hint.Type == ItemType.Palette ||
                        hint.Type == ItemType.Category)

                    e.Graphics.DrawImage(image,
                        indent, e.Bounds.Top);
                    e.Graphics.DrawString(
                        m_cbPalettePath.Items[e.Index].ToString() 
                        + saveIndicator,
                        drawFont, new SolidBrush(textColor), 
                        indent + imageSize.Width,
                        e.Bounds.Top);
                }
            }
        }

        private void m_cbPalettePath_SelectionChangeCommitted(object sender,
            System.EventArgs e)
        {
            int index = m_cbPalettePath.SelectedIndex;
            ComboDrawHint hint = m_comboDrawHints[index] as ComboDrawHint;
            m_currentEntity = null;
            switch (hint.Type)
            {
                case ItemType.Palette:
                {
                    m_currentPalette = hint.Palette;
                    m_currentCategory = string.Empty;
                    break;
                }
                case ItemType.Category:
                {
                    m_currentCategory = hint.CategoryName;
                    break;
                }
                default:
                {
                    return;
                }
            }
            //SyncListView();
            SyncPaletteManagerToDropDown();
            SyncUI();

        }

        private void m_btnMakeTemplateFromSelected_Click(object sender,
            System.EventArgs e)
        {
            //The UI enabler should ensure that this never gets clicked unless 
            //there is one and only one entity slected
            MEntity entity = SelectionService.GetSelectedEntities()[0];
            AddNewTemplateDlg dlg = new AddNewTemplateDlg();
            dlg.TemplateName = entity.Name;
            dlg.SelectedPalette = m_currentPalette;
            dlg.Category = m_currentCategory;

            if (dlg.ShowDialog() == DialogResult.OK)
            {
                MEntity existingTemplate = 
                    m_currentPalette.GetEntityByName(dlg.TemplateName);
                if (existingTemplate != null)
                {
                    //string existingCategory = m_currentPalette
                }
                MEntity newTemplate = entity.Clone(dlg.TemplateName, false);
                newTemplate.MasterEntity = null;
                newTemplate.TemplateID = Guid.NewGuid();
                
                dlg.SelectedPalette.AddEntity(newTemplate, dlg.Category, true);
                dlg.SelectedPalette.ActiveEntity = newTemplate;
                if(FW.PaletteManager.ActivePalette != dlg.SelectedPalette)
                {
                    FW.PaletteManager.ActivePalette = dlg.SelectedPalette;
                }
                CommandPanelService.ShowPanel("Template Properties", true);
                MFramework.Instance.Scene.Dirty = true;
            }
        }

        private void m_btnSmallIconView_Click(object sender,
            System.EventArgs e)
        {
            m_tbbSmallIcons.Enabled = false;
            m_tbbLargeIcons.Enabled = true;

            m_tbbSmallIcons.Pushed = true;
            m_tbbLargeIcons.Pushed = false;


            SettingsService.SetSettingsObject(m_listSettingName, false,
                SettingsCategory.PerUser);

            m_lvPalettes.View = View.List;
        }

        private void m_btnLargeIconView_Click(object sender, System.EventArgs e)
        {
            m_tbbSmallIcons.Enabled = true;
            m_tbbLargeIcons.Enabled = false;

            m_tbbSmallIcons.Pushed = false;
            m_tbbLargeIcons.Pushed = true;


            SettingsService.SetSettingsObject(m_listSettingName, true,
                SettingsCategory.PerUser);

            m_lvPalettes.View = View.LargeIcon;
        
        }

        private void m_btnNewPalette_Click(object sender, System.EventArgs e)
        {
            ArrayList paletteNames = new ArrayList();
            foreach (MPalette palette in FW.PaletteManager.GetPalettes())
            {
                paletteNames.Add(palette.Name);
            }
            AddNewPaletteDlg dlg = new AddNewPaletteDlg(
                paletteNames.ToArray(typeof(string)) as string[]);
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                MPalette newPalette = new MPalette(dlg.PaletteName, 0);
                newPalette.Scene.Dirty = true;
                newPalette.Path = dlg.DirectoryPath;
                if (!FW.PaletteManager.AddPalette(newPalette))
                {
                    MessageBox.Show("�ȷ�Ʈ ����Ʈ ���� �̹� �ش� �ȷ�Ʈ�� �ֽ��ϴ�.");
                    return;
                }
                FW.PaletteManager.ActivePalette = newPalette;

                // �׳� �ٷ� ���� �����ϰ� �����.
                MFramework.Instance.Scene.Dirty = true;
            }
        }

        private void m_btnMoveTemplate_Click(object sender, System.EventArgs e)
        {
            MoveCopyRename(false);
        }

	    private void MoveCopyRename(bool bCheckCopyBox)
	    {
	        AddNewTemplateDlg dlg = new AddNewTemplateDlg();
            dlg.CopyOptionChecked = bCheckCopyBox;
	        dlg.SelectedPalette = m_currentPalette;
	        dlg.Category = m_currentCategory;
	        dlg.TemplateName = 
	            PaletteUtilities.SimpleName(m_currentEntity.Name);
	        dlg.Text = "Copy, Move or Rename Template...";
	        dlg.ShowCopyOption = true;
    
	        if (dlg.ShowDialog() == DialogResult.OK)
	        {
	            MPalette oldPalette = m_currentPalette;
	            string oldCategory = m_currentCategory;
	            MEntity oldEntity = m_currentEntity;
                
	            MPalette newPalette = dlg.SelectedPalette;
	            string newCategory = dlg.Category;
	            string newName;
                if (newCategory != string.Empty)
                {
                    newName = string.Format("[{0}]{1}.{2}", newPalette.Name, 
                        newCategory, dlg.TemplateName);
                }
                else
                {
                    newName = string.Format("[{0}]{1}", newPalette.Name,
                        dlg.TemplateName);
                }

                if (dlg.CopyOptionChecked)
                {

                    CommandService.BeginUndoFrame(
                        string.Format("Copied template '[{0}]{1}'",
                        oldPalette.Name, oldEntity.Name));
                }
                else
                {
                    CommandService.BeginUndoFrame(
                        string.Format("Moved/Renamed template '[{0}]{1}'",
                        oldPalette.Name, oldEntity.Name));
                    
                }
	            if (!dlg.CopyOptionChecked)
	            {
	                oldPalette.RemoveEntity(oldEntity, true);
	                oldEntity.Name = newName;
	                newPalette.AddEntity(oldEntity, string.Empty, true);
	            }
	            else
	            {
	                MEntity newEntity = oldEntity.Clone(newName, false);
	                newEntity.TemplateID = Guid.NewGuid();
	                newPalette.AddEntity(newEntity, string.Empty, true);
                    newPalette.ActiveEntity = newEntity;
	                m_currentEntity = newEntity;
	            }

	            if (dlg.CopyOptionChecked)
	            {
	                FW.PaletteManager.ActivePalette = newPalette;
	            }

                newPalette.ResetEntityNames();

                if (!dlg.CopyOptionChecked)
                {
                    newPalette.ActiveEntity = oldEntity;
	                m_currentEntity = oldEntity;
                }
	            m_currentCategory = newCategory;
	            m_currentPalette = newPalette;
	            SyncUI();
	            if (dlg.CopyOptionChecked)
	            {
	                CommandPanelService.ShowPanel("Template Properties",
                        true);                    
	            }
                CommandService.EndUndoFrame(true);
	        }
	    }

	    private void m_btnDeleteTemplate_Click(object sender,
            System.EventArgs e)
        {
            MEntity templateToDelete = m_currentEntity;
            MEntity[] dependantEntities = 
                FW.Scene.GetDependentEntities(templateToDelete);
            if (dependantEntities.Length > 0)
            {
                
                MessageBox.Show(
                    "Entities in the scene depend on the selected template.\n"
                    + "It will not be deleted.",
                    "Cannot Delete Template");
            }
            else
            {
                foreach(MPalette palette in FW.PaletteManager.GetPalettes())
                {
                    MEntity[] dependantPaletteEntities = 
                        palette.Scene.GetDependentEntities(templateToDelete);
                    if (dependantPaletteEntities.Length > 0)
                    {
                        string warningText = string.Format(
                            "Entities in Palette \"{0}\" depend on the " 
                            + "selected template", palette.Name);
                        
                        MessageBox.Show(warningText, "Delete template...");
                        return;
                    }
                }
                m_currentPalette.RemoveEntity(m_currentEntity, true);
                m_currentPalette.ActiveEntity = null;
            }
        }

        private void m_lvPalettes_KeyDown(object sender, 
            System.Windows.Forms.KeyEventArgs e)
        {
            switch (e.KeyData)
            {
                case Keys.Delete:
                {
                    if (m_tbbDeleteTemplate.Enabled)
                    {
                        m_btnDeleteTemplate_Click(sender, e);
                    }
                    break;
                }
            }
        }

        private void m_btnNewTemplate_Click(object sender, System.EventArgs e)
        {
            AddNewTemplateDlg dlg = new AddNewTemplateDlg();
            dlg.SelectedPalette = m_currentPalette;            
            dlg.Category = m_currentCategory;

            if (dlg.ShowDialog() == DialogResult.OK)
            {
                string fullTemplateName = 
                    AppendPaletteNameAndCategory(dlg.TemplateName, 
                    FW.PaletteManager.ActivePalette, dlg.Category);
                MEntity newEntity = 
                    MEntity.CreateGeneralEntity(fullTemplateName);
                //category name is already included
                //dlg.SelectedPalette.AddEntity(newEntity, string.Empty,
                //    true);      
                FW.PaletteManager.ActivePalette = dlg.SelectedPalette;
                FW.PaletteManager.ActivePalette.ActiveEntity = newEntity;
                CommandPanelService.ShowPanel("Template Properties", true);
            }
        
        }

        private void m_btnRenamePaletteClick(object sender, 
            System.EventArgs e)
        {
            MPaletteManager paletteManager = FW.PaletteManager;
            ArrayList paletteNames = new ArrayList();
            foreach (MPalette palette in paletteManager.GetPalettes())
            {
                paletteNames.Add(palette.Name);
            }
            paletteNames.Add("Unsaved Scene Templates");
            AddNewPaletteDlg dlg = new AddNewPaletteDlg(
                paletteNames.ToArray(typeof(string)) as string[]);
            //AddNewPaletteDlg dlg = new AddNewPaletteDlg();
            dlg.Text = "Rename Palette";
            string selectedPaletteName;
            if (m_currentPalette != null)
            {
                selectedPaletteName = m_currentPalette.Name;
            }
            else
            {
                selectedPaletteName = m_lvPalettes.SelectedItems[0].Text;
            }
            dlg.PaletteName = selectedPaletteName;

            if (dlg.ShowDialog() == DialogResult.OK &&
                !dlg.PaletteName.Equals(selectedPaletteName))
            {
                MPalette palette = 
                    paletteManager.GetPaletteByName(
                    selectedPaletteName);
                RenamePalette(palette, dlg.PaletteName);
            }
        }


	    private void m_btnDeletePaletteClick(object sender, 
            ToolBarButtonClickEventArgs e)
        {
            MPaletteManager paletteManager = FW.PaletteManager;
            string selectedPaletteName;
            if (m_currentPalette != null)
            {
                selectedPaletteName = m_currentPalette.Name;
            }
            else
            {
                selectedPaletteName = m_lvPalettes.SelectedItems[0].Text;
            }
            MPalette palette = 
                paletteManager.GetPaletteByName(
                selectedPaletteName);
            MEntity[] templates = palette.GetEntities();
            foreach(MEntity template in templates)
            {
                MEntity[] dependantEntities = 
                    FW.Scene.GetDependentEntities(template);
                if (dependantEntities.Length > 0)
                {
                
                    MessageBox.Show(
                        "Entities in the scene depend on templates in the" + 
                        " selected palette.\n It will not be deleted.",
                        "Cannot Delete Palette");
                    return;
                }                
            }
            DialogResult result = 
                MessageBox.Show(string.Format("Deleting a palette " +
                "is NOT undoable, are you sure you would like delete '{0}'?",
                palette.Name), "Delete Palette?", MessageBoxButtons.YesNo,
                MessageBoxIcon.Warning);
            if (result == DialogResult.Yes)
            {
                DeletePalette(palette);
            }
        }


        private void m_tbToolBar_ButtonClick(object sender, 
            System.Windows.Forms.ToolBarButtonClickEventArgs e)
        {
            ToolBarButton[] buttons =
                {
                    m_tbbImportPalette,
                    m_tbbNewPalette,                    
                    m_tbbNewTemplate,
                    m_tbbCloneFromEntity,
                    m_tbbMoveCopyRename,
                    m_tbbDeleteTemplate,
                    m_tbbAddArtAssets,
                    m_tbbLargeIcons,
                    m_tbbSmallIcons,
                    m_tbbRenamePalette,
                    m_tbbDeletePalette
                };
            switch (Array.IndexOf(buttons, e.Button))
            {
                case 0: //Import Palette
                {
                    m_btnImportPalette_Click(sender, e);
                    break;
                }
                case 1: //New Palette
                {
                    m_btnNewPalette_Click(sender, e);
                    break;
                }
                case 2: //New Template
                {
                    m_btnNewTemplate_Click(sender, e);
                    break;
                }
                case 3: //Clone From Entity
                {
                    m_btnMakeTemplateFromSelected_Click(sender, e);
                    break;
                }
                case 4: //Move/Copy/Rename
                {
                    m_btnMoveTemplate_Click(sender, e);
                    break;
                }
                case 5: //Delete Template
                {
                    m_btnDeleteTemplate_Click(sender, e);
                    break;
                }
                case 6: //Add art assets
                {
                    UICommandService.GetCommand(
                        "CreateTemplateFromFiles").DoClick(sender, e);
                    break;
                }
                case 7: //LargeIcons
                {
                    m_btnLargeIconView_Click(sender, e);
                    break;
                }
                case 8: //Small Icons
                {
                    m_btnSmallIconView_Click(sender, e);
                    break;
                }
                case 9: //Rename Palette
                {
                    m_btnRenamePaletteClick(sender, e);
                    break;
                }
                case 10: //Delete Palette
                {
                    m_btnDeletePaletteClick(sender, e);
                    break;
                }
                default:
                {
                    throw new InvalidOperationException(
                        "An invalid ToolBar Button index was encountered");
                }
            }
        }

	    private void m_lvPalettes_MouseUp(object sender, MouseEventArgs e)
        {
            if ((e.Button & MouseButtons.Right) == MouseButtons.Right)
            {
                if (m_lvPalettes.SelectedItems.Count > 0)
                {
                    ListViewItem item = m_lvPalettes.SelectedItems[0];
                    ItemType itemType = (ItemType) item.Tag;
                    switch (itemType)
                    {
                        case ItemType.Template:
                        {
                            m_contextMenu.Show(m_lvPalettes, 
                                new Point(e.X,  e.Y));
                            break;
                        }
                        default:
                        {
                            break;
                        }
                    }
                }
            }
        
        }

        private void m_menuMoveRename_Click(object sender, System.EventArgs e)
        {
            MoveCopyRename(false);
        }

        private void m_menuCopy_Click(object sender, System.EventArgs e)
        {
            MoveCopyRename(true);
        
        }

        private void m_menuDelete_Click(object sender, System.EventArgs e)
        {
            m_btnDeleteTemplate_Click(sender, e);
        
        }

        private void m_menuSelectInScene_Click(object sender, 
            System.EventArgs e)
        {
            MScene mainScene = MFramework.Instance.Scene;
            ArrayList selectionList = new ArrayList();
            MEntity entityToFind = m_currentEntity;
            MEntity[] dependantEntities = 
                mainScene.GetDependentEntities(entityToFind);
            foreach (MEntity entity in dependantEntities)
            {
                if (mainScene.IsEntityInScene(entity))
                {
                    selectionList.Add(entity);
                }
            }
            if (selectionList.Count > 0)
            {
                ISelectionService selectionService = 
                    ServiceProvider.Instance.GetService(
                    typeof(ISelectionService)) as ISelectionService;
                selectionService.AddEntitiesToSelection(
                    selectionList.ToArray(typeof(MEntity)) as MEntity[]);
            }
        }

        private void m_menuProperties_Click(object sender, System.EventArgs e)
        {
            ICommandPanelService panelService = 
                ServiceProvider.Instance.GetService(
                typeof(ICommandPanelService)) as ICommandPanelService;
            Form panel = panelService.GetPanel("Template Properties");
            if (panel != null)
            {
                panelService.ShowPanel("Template Properties", true);
            }
        }

        #endregion


        #region UI element refresh code

        private void SyncUI()
        {
            SyncDropDown();
            SyncListView();
        }

        private void SyncPaletteManagerToListView()
        {
            if(!m_bIsSyncingListView)
            {
                m_bIsSyncingPaletteManagerToListView = true;
                DoPaletteSync();
                m_bIsSyncingPaletteManagerToListView = false;
            }
        }

        /// <summary>
        /// Called only by the SyncPaletteManager* methods
        /// Do not call directly
        /// </summary>
	    private void DoPaletteSync()
	    {
            MPalette oldPalette = FW.PaletteManager.ActivePalette;
            if (m_currentPalette != oldPalette)
            {
                FW.PaletteManager.ActivePalette = m_currentPalette;
            }
            if (FW.PaletteManager.ActivePalette != null)
            {
                if (FW.Monster.IsMonsterData(FW.PaletteManager.ActivePalette.Name))
                    return;

                MEntity oldTemplate = 
                    FW.PaletteManager.ActivePalette.ActiveEntity;
                if (m_currentEntity != oldTemplate && m_currentEntity != null)
                {
                    FW.PaletteManager.ActivePalette.ActiveEntity = 
                        m_currentEntity;
                }
            }
	    }

	    private void SyncPaletteManagerToDropDown()
        {
            if(!m_bIsSyncingDropDown)
            {
                m_bIsSyncingPaletteManagerToDropDown = true;
                DoPaletteSync();
                m_bIsSyncingPaletteManagerToDropDown = false;
            }
        }


        private void SyncListView()
        {
            if (!m_bIsSyncingPaletteManagerToListView)
            {
                m_bIsSyncingListView = true;
                if (m_currentPalette == null)
                {
                    RefreshListViewTopLevel();
                }
                else
                {
                    m_lvPalettes.Items.Clear();
                    string[] categories = 
                        PaletteUtilities.BuildSubcategories(m_currentPalette, 
                        m_currentCategory);
                    ListViewItem upDirectoryItem = m_lvPalettes.Items.Add(
                        " ..", (int) ItemType.Category);
                    upDirectoryItem.Tag = ItemType.Category;
                    foreach (string subCategory in categories)
                    {
                        ListViewItem item = m_lvPalettes.Items.Add(" " +
                            PaletteUtilities.SimpleName(subCategory),
                            (int) ItemType.Category);
                        item.Tag = ItemType.Category;
                    
                    }
                    MEntity[] currentEntities =
                        BuildEntitiesInCategory(m_currentPalette,
                        m_currentCategory);
                    foreach (MEntity entity in currentEntities)
                    {
                        ListViewItem item = m_lvPalettes.Items.Add(
                            PaletteUtilities.SimpleName(entity.Name), 
                            (int) ItemType.Template);
                        item.Tag = ItemType.Template;
                        if (entity == m_currentEntity)
                        {
                            item.Selected = true;
                        }
                    }
                }
                m_bIsSyncingListView = false;
            }
        }

        private void RefreshListViewTopLevel()
        {
            m_lvPalettes.Items.Clear();
            MFramework fw = MFramework.Instance;
            MPalette[] palettes = fw.PaletteManager.GetPalettes();
            foreach (MPalette palette in palettes)
            {
                ListViewItem item = m_lvPalettes.Items.Add(palette.Name , 0);
                if (palette.Scene.Dirty)
                {
                    item.Font = new Font(item.Font, FontStyle.Bold);
                }
                if (palette.DontSave)
                {
                    item.ForeColor = m_cOrphanColor;
                }
                
                item.Tag = ItemType.Palette;
            }        	        
        }

        private void SyncDropDown()
        {
            if (!m_bIsSyncingPaletteManagerToDropDown)
            {
                m_bIsSyncingDropDown = true;
                MPalette[] palettes = FW.PaletteManager.GetPalettes();
                m_cbPalettePath.Items.Clear();
                ArrayList hints = new ArrayList();
                int indexToSelect = 0;

                //Add "Root" Level
                m_cbPalettePath.Items.Add("All Palettes");
                ComboDrawHint rootHint = new ComboDrawHint();
                rootHint.Type = ItemType.Palette;
                rootHint.Palette = null;            
                hints.Add(rootHint);
                int itemCounter = 1;
                bool bCurrentCheck = false;
                foreach (MPalette palette in palettes)
                {
                    m_cbPalettePath.Items.Add(palette.Name);
                    ComboDrawHint hint = new ComboDrawHint();
                    hint.Type = ItemType.Palette;
                    hint.indent = rootHint.indent + 5;
                    hint.Palette = palette;
                    hints.Add(hint);                
                    itemCounter++;
                    if (palette == m_currentPalette)
                    {
                        bCurrentCheck = true;
                        indexToSelect = itemCounter - 1;
                        int currentIndent = hint.indent;
                        if (!m_currentCategory.Equals(string.Empty))
                        {
                            string[] categories = m_currentCategory.Split('.');
                            ArrayList localCategory = new ArrayList();
                            foreach (string category in categories)
                            {
                                localCategory.Add(category);
                                m_cbPalettePath.Items.Add(category);
                                ComboDrawHint categoryHint = 
                                    new ComboDrawHint();                        
                                categoryHint.Type = ItemType.Category;
                                currentIndent += 5;
                                categoryHint.indent = currentIndent;
                                categoryHint.CategoryName = 
                                    PaletteUtilities.FullCategoryName(
                                    localCategory.ToArray(typeof(string))
                                    as string[]);
                                categoryHint.Palette = palette;
                                hints.Add(categoryHint);
                                itemCounter++;
                                indexToSelect = itemCounter - 1;
                            }
                        }
                    }
                }

                if (!bCurrentCheck)
                {
                    m_currentPalette = null;
                }

                m_comboDrawHints = hints;
                m_cbPalettePath.SelectedIndex = indexToSelect;
                m_bIsSyncingDropDown = false;
                m_ttToolTip.SetToolTip(m_cbPalettePath,
                    string.Format("Palette Folder: {0}", 
                    FW.PaletteManager.PaletteFolder));
            }
        }

        #endregion

        #region Framework Utilities

	    private MEntity[] BuildEntitiesInCategory(MPalette palette,
            string category)
	    {
            ArrayList entityList = new ArrayList();
            MEntity[] entities = palette.GetEntities();
            foreach (MEntity entity in entities)
            {
                string entityCategory = MPalette.StripPaletteName(
                    palette.GetEntityCategory(entity));
                if (category.Equals(entityCategory))
                {
                    entityList.Add(entity);
                }
            }
            return entityList.ToArray(typeof(MEntity)) as MEntity[];
	    }

        #endregion

        #region Utility Methods
        private void DeletePalette(MPalette palette)
        {
            string palettePath = 
                MFramework.Instance.PaletteManager.PaletteFolder +
                palette.Name + ".pal";
            FileInfo fi = new FileInfo(palettePath);
            if (fi.Exists)
            {
                if ((fi.Attributes & FileAttributes.ReadOnly) == 
                    FileAttributes.ReadOnly)
                {
                    DialogResult result = 
                        MessageBox.Show("Palette file is read only,\n" +
                        " are you sure you want to delete it?",
                        "Delete Palette?", MessageBoxButtons.YesNo,
                        MessageBoxIcon.Question);
                    if (result == DialogResult.Yes)
                    {
                        fi.Attributes = 
                            fi.Attributes & ~FileAttributes.ReadOnly;
                        
                    }
                    else
                    {
                        return;
                    }

                }
                //fi.Delete();
            }
            MFramework.Instance.PaletteManager.RemovePalette(palette);
        }

        private void RenamePalette(MPalette palette, string name)
        {
            
            string paletteFolder = FW.PaletteManager.PaletteFolder;
                
            string oldPalettePath = paletteFolder + palette.Name + ".pal";
            string newPalettePath = paletteFolder + name + ".pal";
            FileInfo fiNew = new FileInfo(newPalettePath);
            FileInfo fiOld = new FileInfo(oldPalettePath);
            if (fiNew.Exists)
            {
                MessageBox.Show("New name alread exisits!");
                return;
            }
            if (fiOld.Exists)
            {
                if ((fiOld.Attributes & FileAttributes.ReadOnly) == 
                    FileAttributes.ReadOnly)
                {
                    DialogResult result = 
                        MessageBox.Show("Palette file is read only,\n" +
                        " are you sure you want to rename it?",
                        "Rename Palette?", MessageBoxButtons.YesNo,
                        MessageBoxIcon.Question);
                    if (result == DialogResult.Yes)
                    {
                        fiOld.Attributes = 
                            fiOld.Attributes & ~FileAttributes.ReadOnly;
                        
                    }
                    else
                    {
                        return;
                    }

                }
                fiOld.Attributes = 
                    fiOld.Attributes & ~FileAttributes.ReadOnly;
            }
            if (palette.Scene.Dirty || palette.DontSave)
            {
                MFramework.Instance.PaletteManager.SavePalette(
                    palette, oldPalettePath, "GSA");
            }
            try
            {
                fiOld.CopyTo(newPalettePath, false);
                //fiOld.Delete();
            }
            catch (IOException)
            {
                throw;
            }

            FW.PaletteManager.RemovePalette(palette);

            MPalette newPalette = 
                MFramework.Instance.PaletteManager.LoadPalette(newPalettePath);

            PaletteUtilities.CheckForDuplicateTemplates(newPalette,
                MFramework.Instance.PaletteManager.GetPalettes());

            FW.PaletteManager.AddPalette(newPalette);            
            
        }


        #endregion

        #region Framework Event Handlers

        private void EventManager_PaletteAdded(MPalette pmPalette)
        {
            SyncUI();
        }

        private void EventManager_PaletteRemoved(MPalette pmPalette)
        {
            SyncUI();
        }

        private void EventManager_ActivePaletteChanged(
            MPalette pmActivePalette, MPalette pmOldActivePalette)
        {
            if (m_currentPalette != pmActivePalette)
            {
                m_currentPalette = pmActivePalette;
                m_currentCategory = string.Empty;
                if (pmActivePalette != null)
                {
                    m_currentEntity = pmActivePalette.ActiveEntity;
                    if (m_currentEntity != null)
                    {
                        m_currentCategory = MPalette.StripPaletteName(
                            m_currentPalette.GetEntityCategory(
                            m_currentEntity));                
                    }
                }

                else
                {
                    m_currentEntity = null;
                }
                SyncUI();
            }
        }

        private void EventManager_PaletteActiveEntityChanged(
            MPalette pmPalette, MEntity pmOldActiveEntity)
        {
            //m_currentEntity = null;
            if (pmPalette == m_currentPalette)
            {
                m_currentEntity = m_currentPalette.ActiveEntity;
                if (m_currentEntity != null)
                {
                    m_currentCategory = 
                        MPalette.StripPaletteName(
                        m_currentPalette.GetEntityCategory(m_currentEntity));
                }
                else
                {
                    //m_currentCategory = "";
                }
                SyncUI();
            }
        }

        private void EventManager_EntityAddedToPalette(MScene pmScene,
                                                       MEntity pmEntity)
        {
            if (m_currentPalette != null && 
                m_currentPalette.Scene == pmScene)
            {
                SyncUI();
            }

        }

        private void EventManager_EntityRemovedFromPalette(MScene pmScene,
                                                           MEntity pmEntity)
        {
            if (m_currentPalette != null && 
                m_currentPalette.Scene == pmScene)
            {
                SyncUI();
            }
        }

        #endregion

        [UICommandHandler("Idle")]
        private void OnIdle(object sender, EventArgs args)
        {
            //Only enable "From selected entities" if there is a selection

            bool bPaletteIsActive = FW.PaletteManager.ActivePalette != null;            

            bool bPaletteIsActiveAndSavable = bPaletteIsActive &&
                !FW.PaletteManager.ActivePalette.DontSave;    

            bool bSaveablePalettesExist = false;

            bool bPaletteisSelected = m_currentPalette == null && 
                m_lvPalettes.SelectedItems.Count != 0;
        
            MPalette[] palettes = FW.PaletteManager.GetPalettes();

            foreach(MPalette palette in palettes)
            {
                if (!palette.DontSave)
                {
                    bSaveablePalettesExist = true;
                    break;
                }
            }

            bool bTemplateIsActive = bPaletteIsActive
                && (FW.PaletteManager.ActivePalette.ActiveEntity != null);

            m_tbbMoveCopyRename.Enabled = bTemplateIsActive && 
                bSaveablePalettesExist;


            m_tbbDeleteTemplate.Enabled = bTemplateIsActive && 
                bPaletteIsActiveAndSavable;

            m_tbbCloneFromEntity.Enabled = bPaletteIsActiveAndSavable &&
                SelectionService.GetSelectedEntities().Length == 1;


            m_tbbNewTemplate.Enabled = bPaletteIsActiveAndSavable;

            m_tbbAddArtAssets.Enabled = bPaletteIsActiveAndSavable;


            m_tbbDeletePalette.Enabled = bPaletteIsActive || 
                bPaletteisSelected;

            m_tbbRenamePalette.Enabled = bPaletteIsActive ||
                bPaletteisSelected;


            if (MFramework.Instance.PaletteManager.Refresh)
            {
                RefreshListViewTopLevel();
                MFramework.Instance.PaletteManager.Refresh = false;
            }
        }

        [UICommandValidator("CreateTemplateFromFiles")]
        private void OnValidateCreateTemplateFromFiles(object sender, 
            UIState state)
        {
            bool bPaletteIsActive = FW.PaletteManager.ActivePalette != null;            

            bool bPaletteIsActiveAndSavable = bPaletteIsActive &&
                !FW.PaletteManager.ActivePalette.DontSave;    

            state.Enabled = bPaletteIsActiveAndSavable;
        }

        [UICommandHandler("CreateTemplateFromFiles")]
        private void OnCreateTemplateFromFiles(object pmObject,
            EventArgs pmEventArgs)
        {
            OpenFileDialog pmOpenFileDialog = new OpenFileDialog();
            pmOpenFileDialog.Title = "Choose Art Assets to Add to Palette";
            pmOpenFileDialog.Filter = "All Files (*.*)|*.*";
            pmOpenFileDialog.Multiselect = true;
            if (pmOpenFileDialog.ShowDialog() == DialogResult.OK)
            {
                AddNewTemplateDlg pmTemplateDialog = new AddNewTemplateDlg();
                pmTemplateDialog.SelectedPalette = m_currentPalette;
                pmTemplateDialog.Category = m_currentCategory;
                pmTemplateDialog.ShowCopyOption = false;
                pmTemplateDialog.TemplateName = "[Multiple Templates]";
                pmTemplateDialog.TemplateNameEnabled = false;
                if (pmTemplateDialog.ShowDialog() == DialogResult.OK)
                {
                    CommandService.BeginUndoFrame(
                        "Add art assets to palette");

                    MPalette pmPalette = pmTemplateDialog.SelectedPalette;
                    string strCategory = pmTemplateDialog.Category;

                    MEntity pmNewTemplate = null;
                    foreach (string strFileName in pmOpenFileDialog.FileNames)
                    {
                        pmNewTemplate = MPalette.CreateTemplateFromFile(
                            strFileName);
                        if (pmNewTemplate != null)
                        {
                            string fullTemplateName = 
                                AppendPaletteNameAndCategory(
                                pmNewTemplate.Name, 
                                FW.PaletteManager.ActivePalette, 
                                pmTemplateDialog.Category);
                            pmNewTemplate.Name = fullTemplateName;
                            
                            // Add the new template to the palette.
                            pmPalette.AddEntity(pmNewTemplate, string.Empty,
                                true);

							// Add Animation Object Component
							FW.PgUtil.AddAniComponentToEntity(pmNewTemplate, null, true);

                            // Add HideObject Component.
                            FW.PgUtil.AddHideObjectComponentToEntity(pmNewTemplate, null, true);
                        }
                    }

                    FW.PaletteManager.ActivePalette = pmPalette;
                    pmPalette.ActiveEntity = pmNewTemplate;

                    m_currentCategory = strCategory;
                    m_currentEntity = pmNewTemplate;
                    m_currentPalette = pmPalette;
                    SyncUI();

                    CommandService.EndUndoFrame(true);
                }
            }
        }

        private void m_timer_Tick(object sender, System.EventArgs e)
        {            
            MFramework fw = MFramework.Instance;
            if (!m_cbPalettePath.DroppedDown)
            {
                m_cbPalettePath.Invalidate();
            }
            {
                //See if a refresh should be triggered
                int iDirtyPaletteCount = 0;
                foreach (MPalette palette in fw.PaletteManager.GetPalettes())
                {
                    if (palette.Scene.Dirty)
                    {
                        iDirtyPaletteCount++;
                    }
                }
                if (iDirtyPaletteCount != m_iLastDirtyPaletteCount)
                {
                    SyncListView();
                    m_iLastDirtyPaletteCount = iDirtyPaletteCount;
                }
            }
        }

    }
}


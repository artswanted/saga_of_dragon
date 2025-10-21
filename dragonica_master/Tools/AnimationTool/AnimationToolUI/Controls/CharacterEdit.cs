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
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using System.Diagnostics;
using System.IO;
using Microsoft.Win32;

using NiManagedToolInterface;

namespace AnimationToolUI
{
	/// <summary>
	/// Summary description for CharacterEdit.
	/// </summary>
	public class CharacterEdit : System.Windows.Forms.UserControl
	{
        #region Delegates

        private MAnimation.__Delegate_OnModelPathAndRootChanged m_OnModelPathAndRootChanged = null;

        private void RegisterDelegates()
        {
            UnregisterDelegates();

            // Create delegates.
            m_OnModelPathAndRootChanged = new MAnimation.__Delegate_OnModelPathAndRootChanged(OnModelPathAndRootChanged);

            // Register delegates.
            MFramework.Instance.Animation.OnModelPathAndRootChanged += m_OnModelPathAndRootChanged;
        }

        private void UnregisterDelegates()
        {
            if(m_OnModelPathAndRootChanged != null)
                MFramework.Instance.Animation.OnModelPathAndRootChanged -= m_OnModelPathAndRootChanged;
        }

        #endregion

        private System.Windows.Forms.Button btnEditRoot;
        private System.Windows.Forms.Label lblSelectedRoot;
		private System.Windows.Forms.Button btnSelectFile;
        private System.Windows.Forms.TextBox tbRoot;
        private System.Windows.Forms.TextBox tbFile;
        private System.Windows.Forms.ToolTip ttCharacterEdit;
        private System.ComponentModel.IContainer components;

        private System.Windows.Forms.ListView lvPartsList;
        private ColumnSort m_kColumnSorter = new ColumnSort();
        private System.Windows.Forms.ColumnHeader chPartsName;
        private System.Windows.Forms.ColumnHeader chFileName;
        private System.Windows.Forms.ToolTip ttHelp;

        private System.Windows.Forms.ContextMenu cmItemView;
        private System.Windows.Forms.MenuItem miNewItem;
        private System.Windows.Forms.MenuItem miModifyItem;
        private System.Windows.Forms.MenuItem miDeleteItem;
        private System.Windows.Forms.MenuItem miRefreshItem;
        private System.Windows.Forms.MenuItem miRefreshThisFolder;
        private System.Windows.Forms.MenuItem miViewItemSceneGraph;

        private System.Windows.Forms.TreeView tvRootSel;
        private System.Windows.Forms.Button btnSelectItemFolder;

        private static uint NumInstances = 0;
        private static string m_kItemFolderRootPath = new string(' ', 0);
        private static string m_kItemFolderSelected = new string(' ', 0);
        private static TreeNode m_kTreeNode = null;
		private static TreeNode m_kDefaultNode = new TreeNode("Default");
		private Button btnRandomGenerate;
		private ComboBox cbSelectGender;
		private ComboBox cbSelectClass;
        private static TreeNode m_kSelectedTreeNode = null;

		public CharacterEdit()
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();
            Debug.Assert(NumInstances == 0);
            NumInstances++;

			lvPartsList.ListViewItemSorter = m_kColumnSorter;
		}

		/// <summary> 
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
                if (m_kTreeNode != null)
                {
                    tvRootSel.Nodes.Remove(m_kTreeNode);
                }
                if (m_kDefaultNode != null)
                {
                    tvRootSel.Nodes.Remove(m_kDefaultNode);
                }
                
                NumInstances--;
                Debug.Assert(NumInstances == 0, "There should only ever be one un-disposed" + 
                    " version of this class in memory at a time.");
                UnregisterDelegates();
				if(components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Component Designer generated code
		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			this.lvPartsList = new System.Windows.Forms.ListView();
			this.chPartsName = new System.Windows.Forms.ColumnHeader();
			this.cmItemView = new System.Windows.Forms.ContextMenu();
			this.miNewItem = new System.Windows.Forms.MenuItem();
			this.miModifyItem = new System.Windows.Forms.MenuItem();
			this.miDeleteItem = new System.Windows.Forms.MenuItem();
			this.miViewItemSceneGraph = new System.Windows.Forms.MenuItem();
			this.miRefreshItem = new System.Windows.Forms.MenuItem();
			this.miRefreshThisFolder = new System.Windows.Forms.MenuItem();
			this.chFileName = new System.Windows.Forms.ColumnHeader();
			this.btnEditRoot = new System.Windows.Forms.Button();
			this.lblSelectedRoot = new System.Windows.Forms.Label();
			this.btnSelectFile = new System.Windows.Forms.Button();
			this.tbRoot = new System.Windows.Forms.TextBox();
			this.tbFile = new System.Windows.Forms.TextBox();
			this.ttCharacterEdit = new System.Windows.Forms.ToolTip(this.components);
			this.tvRootSel = new System.Windows.Forms.TreeView();
			this.btnSelectItemFolder = new System.Windows.Forms.Button();
			this.ttHelp = new System.Windows.Forms.ToolTip(this.components);
			this.btnRandomGenerate = new System.Windows.Forms.Button();
			this.cbSelectGender = new System.Windows.Forms.ComboBox();
			this.cbSelectClass = new System.Windows.Forms.ComboBox();
			this.SuspendLayout();
			// 
			// lvPartsList
			// 
			this.lvPartsList.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.lvPartsList.CheckBoxes = true;
			this.lvPartsList.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.chPartsName});
			this.lvPartsList.ContextMenu = this.cmItemView;
			this.lvPartsList.FullRowSelect = true;
			this.lvPartsList.GridLines = true;
			this.lvPartsList.HideSelection = false;
			this.lvPartsList.Location = new System.Drawing.Point(24, 432);
			this.lvPartsList.Name = "lvPartsList";
			this.lvPartsList.Size = new System.Drawing.Size(202, 102);
			this.lvPartsList.TabIndex = 0;
			this.lvPartsList.UseCompatibleStateImageBehavior = false;
			this.lvPartsList.View = System.Windows.Forms.View.Details;
			this.lvPartsList.ItemCheck += new System.Windows.Forms.ItemCheckEventHandler(this.lvPartsList_ItemCheck);
			this.lvPartsList.ColumnClick += new System.Windows.Forms.ColumnClickEventHandler(this.lvPartsList_ColumnClick);
			// 
			// chPartsName
			// 
			this.chPartsName.Text = "Item";
			this.chPartsName.Width = 150;
			// 
			// cmItemView
			// 
			this.cmItemView.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
            this.miNewItem,
            this.miModifyItem,
            this.miDeleteItem,
            this.miViewItemSceneGraph,
            this.miRefreshItem,
            this.miRefreshThisFolder});
			this.cmItemView.Popup += new System.EventHandler(this.cmItemView_Popup);
			// 
			// miNewItem
			// 
			this.miNewItem.Index = 0;
			this.miNewItem.Text = "Create &New Item";
			this.miNewItem.Click += new System.EventHandler(this.miNewItem_Click);
			// 
			// miModifyItem
			// 
			this.miModifyItem.Index = 1;
			this.miModifyItem.Text = "&Modify Item";
			this.miModifyItem.Click += new System.EventHandler(this.miModifyItem_Click);
			// 
			// miDeleteItem
			// 
			this.miDeleteItem.Index = 2;
			this.miDeleteItem.Text = "&Delete Item";
			this.miDeleteItem.Click += new System.EventHandler(this.miDeleteItem_Click);
			// 
			// miViewItemSceneGraph
			// 
			this.miViewItemSceneGraph.Index = 3;
			this.miViewItemSceneGraph.Text = "&View SceneGraph";
			this.miViewItemSceneGraph.Click += new System.EventHandler(this.miViewItemSceneGraph_Click);
			// 
			// miRefreshItem
			// 
			this.miRefreshItem.Index = 4;
			this.miRefreshItem.Text = "&Refresh Item";
			this.miRefreshItem.Click += new System.EventHandler(this.miRefreshItem_Click);
			// 
			// miRefreshThisFolder
			// 
			this.miRefreshThisFolder.Index = 5;
			this.miRefreshThisFolder.Text = "&Refresh This Folder";
			this.miRefreshThisFolder.Click += new System.EventHandler(this.miRefreshThisFolder_Click);
			// 
			// btnEditRoot
			// 
			this.btnEditRoot.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.btnEditRoot.FlatStyle = System.Windows.Forms.FlatStyle.System;
			this.btnEditRoot.Location = new System.Drawing.Point(24, 88);
			this.btnEditRoot.Name = "btnEditRoot";
			this.btnEditRoot.Size = new System.Drawing.Size(202, 24);
			this.btnEditRoot.TabIndex = 4;
			this.btnEditRoot.Text = "Edit Selected Root...";
			this.ttCharacterEdit.SetToolTip(this.btnEditRoot, "Select the actor root from the available nodes in the scene graph.");
			this.btnEditRoot.Click += new System.EventHandler(this.btnEditRoot_Click);
			// 
			// lblSelectedRoot
			// 
			this.lblSelectedRoot.Location = new System.Drawing.Point(22, 59);
			this.lblSelectedRoot.Name = "lblSelectedRoot";
			this.lblSelectedRoot.Size = new System.Drawing.Size(90, 16);
			this.lblSelectedRoot.TabIndex = 2;
			this.lblSelectedRoot.Text = "Selected Root:";
			// 
			// btnSelectFile
			// 
			this.btnSelectFile.FlatStyle = System.Windows.Forms.FlatStyle.System;
			this.btnSelectFile.Location = new System.Drawing.Point(24, 18);
			this.btnSelectFile.Name = "btnSelectFile";
			this.btnSelectFile.Size = new System.Drawing.Size(80, 24);
			this.btnSelectFile.TabIndex = 0;
			this.btnSelectFile.Text = "Select File...";
			this.ttCharacterEdit.SetToolTip(this.btnSelectFile, "Select the NIF file for use with the current actor manager.");
			this.btnSelectFile.Click += new System.EventHandler(this.btnSelectFile_Click);
			// 
			// tbRoot
			// 
			this.tbRoot.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.tbRoot.Location = new System.Drawing.Point(112, 56);
			this.tbRoot.Name = "tbRoot";
			this.tbRoot.ReadOnly = true;
			this.tbRoot.Size = new System.Drawing.Size(114, 21);
			this.tbRoot.TabIndex = 3;
			this.ttCharacterEdit.SetToolTip(this.tbRoot, "The name of the node in the scene graph to which the actor manager is attached.");
			// 
			// tbFile
			// 
			this.tbFile.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.tbFile.Location = new System.Drawing.Point(112, 18);
			this.tbFile.Name = "tbFile";
			this.tbFile.ReadOnly = true;
			this.tbFile.Size = new System.Drawing.Size(114, 21);
			this.tbFile.TabIndex = 1;
			this.ttCharacterEdit.SetToolTip(this.tbFile, "The path to the NIF file used to specify the model geometry for the actor manager" +
					".");
			// 
			// tvRootSel
			// 
			this.tvRootSel.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.tvRootSel.HideSelection = false;
			this.tvRootSel.Location = new System.Drawing.Point(24, 172);
			this.tvRootSel.Name = "tvRootSel";
			this.tvRootSel.Size = new System.Drawing.Size(202, 224);
			this.tvRootSel.TabIndex = 10;
			this.ttCharacterEdit.SetToolTip(this.tvRootSel, "Select the desired actor root node here\nfrom the scene graph in the NIF file.");
			this.tvRootSel.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.tvRootSel_AfterSelect);
			// 
			// btnSelectItemFolder
			// 
			this.btnSelectItemFolder.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.btnSelectItemFolder.FlatStyle = System.Windows.Forms.FlatStyle.System;
			this.btnSelectItemFolder.Location = new System.Drawing.Point(24, 118);
			this.btnSelectItemFolder.Name = "btnSelectItemFolder";
			this.btnSelectItemFolder.Size = new System.Drawing.Size(202, 24);
			this.btnSelectItemFolder.TabIndex = 4;
			this.btnSelectItemFolder.Text = "Select Item Folder";
			this.ttCharacterEdit.SetToolTip(this.btnSelectItemFolder, "Select the Item\'s Base Folder");
			this.btnSelectItemFolder.Click += new System.EventHandler(this.btnSelectItemFolder_Click);
			// 
			// btnRandomGenerate
			// 
			this.btnRandomGenerate.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.btnRandomGenerate.Location = new System.Drawing.Point(24, 403);
			this.btnRandomGenerate.Name = "btnRandomGenerate";
			this.btnRandomGenerate.Size = new System.Drawing.Size(202, 23);
			this.btnRandomGenerate.TabIndex = 11;
			this.btnRandomGenerate.Text = "I\'m feeling lucky";
			this.btnRandomGenerate.UseVisualStyleBackColor = true;
			this.btnRandomGenerate.Click += new System.EventHandler(this.btnRandomGenerate_Click);
			// 
			// cbSelectGender
			// 
			this.cbSelectGender.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.cbSelectGender.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cbSelectGender.FormattingEnabled = true;
			this.cbSelectGender.Items.AddRange(new object[] {
            "Male",
            "Female"});
			this.cbSelectGender.Location = new System.Drawing.Point(24, 146);
			this.cbSelectGender.Name = "cbSelectGender";
			this.cbSelectGender.Size = new System.Drawing.Size(100, 20);
			this.cbSelectGender.TabIndex = 15;
			this.cbSelectGender.SelectedIndex = 0;
			// 
			// cbSelectClass
			// 
			this.cbSelectClass.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.cbSelectClass.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cbSelectClass.FormattingEnabled = true;
			this.cbSelectClass.Items.AddRange(new object[] {
            "Fighter",
            "Magician",
            "Archer",
            "Thief"});
			this.cbSelectClass.Location = new System.Drawing.Point(130, 146);
			this.cbSelectClass.Name = "cbSelectClass";
			this.cbSelectClass.Size = new System.Drawing.Size(96, 20);
			this.cbSelectClass.TabIndex = 15;
			this.cbSelectClass.SelectedIndex = 0;
			// 
			// CharacterEdit
			// 
			this.BackColor = System.Drawing.SystemColors.Control;
			this.Controls.Add(this.cbSelectClass);
			this.Controls.Add(this.cbSelectGender);
			this.Controls.Add(this.btnRandomGenerate);
			this.Controls.Add(this.tbFile);
			this.Controls.Add(this.tbRoot);
			this.Controls.Add(this.lblSelectedRoot);
			this.Controls.Add(this.btnSelectFile);
			this.Controls.Add(this.btnSelectItemFolder);
			this.Controls.Add(this.tvRootSel);
			this.Controls.Add(this.btnEditRoot);
			this.Controls.Add(this.lvPartsList);
			this.Name = "CharacterEdit";
			this.Padding = new System.Windows.Forms.Padding(5);
			this.Size = new System.Drawing.Size(250, 557);
			this.Load += new System.EventHandler(this.CharacterEdit_Load);
			this.ResumeLayout(false);
			this.PerformLayout();

        }
		#endregion

		private void CharacterEdit_Load(object sender, System.EventArgs e)
		{
            RegisterDelegates();

            if (m_kItemFolderRootPath == null || m_kItemFolderRootPath.Length == 0)
            {
				string kPath = "";
				if (AppUtils.RestoreRegKey("ItemRootPath", ref kPath))
				{
					BuildTreeView(kPath);
				}
            }
            UpdateContents();

            MFramework.Instance.Animation.SetPlaybackMode(
                MAnimation.PlaybackMode.None, null);
		}

        private void OnModelPathAndRootChanged(string strModelPath,
            string strModelRoot)
        {
            UpdateContents();
        }

        private void StoreItemRootPath(string kItemRootPath)
        {
            // ItemRootPath를 Registry에 저장한다.
            string kRegistryPath = "Software\\" + Application.CompanyName + "\\" +
                Application.ProductName;

            RegistryKey key = Registry.CurrentUser.CreateSubKey(
                kRegistryPath + "\\ItemSetting");

            key.SetValue("ItemRootPath", kItemRootPath);
        }

        private bool RestoreItemRootPath()
        {
            // Registry에 있는 ItemRootPath를 가져온다.
            string kRegistryPath = "Software\\" + Application.CompanyName + "\\" +
                Application.ProductName;

            RegistryKey key = Registry.CurrentUser.OpenSubKey(
               kRegistryPath + "\\ItemSetting");

            if (key == null)
            {
                return false;
            }

            string kPath = (string)key.GetValue("ItemRootPath", string.Empty);
			BuildTreeView(kPath);
            return true;
        }

        private void btnSelectFile_Click(object sender, System.EventArgs e)
        {
            AnimationHelpers.OpenModelDialog(this.ParentForm);
        }

        private void btnSelectItemFolder_Click(object sender, System.EventArgs e)
        {
            if(AnimationHelpers.OpenFolderDialog(this.ParentForm, ref m_kItemFolderRootPath))
            {
                BuildTreeView(m_kItemFolderRootPath);
            }
        }

		//private void btnAddParts_Click(object sender, System.EventArgs e)
		//{
		//    if(AnimationHelpers.OpenPartDialog(this.ParentForm))
		//    {
		//        m_kSelectedTreeNode = m_kDefaultNode;
		//        UpdateContents();
		//    }
		//}

		//private void btnRemoveParts_Click(object sender, System.EventArgs e)
		//{
		//    if (AnimationHelpers.OpenPartRemoveDialog(this.ParentForm, ref lvPartsList, m_kItemFolderSelected))
		//    {
		//        UpdateContents();
		//    }
		//}

        public void BuildTreeView(string kPath)
        {
            // TreeView Infomation Clean Up!
            // Update Tree View
            if (kPath.Length != 0)
            {
				if (!System.IO.Directory.Exists(kPath))
				{
					return;
				}
				
				m_kItemFolderRootPath = kPath;
				AppUtils.StoreRegKey("ItemRootPath", kPath);

                System.IO.DirectoryInfo kDir = new System.IO.DirectoryInfo(kPath);
                TreeNode kSelectedNode = new TreeNode();
                TreeNode kTreeNode = CreateNodesFromDirectory(ref kDir, ref kSelectedNode);

                // Now we populate tree from kScene
                tvRootSel.Nodes.Clear();
                if (m_kDefaultNode != null)
                {
                    tvRootSel.Nodes.Add(m_kDefaultNode);
                }

                if (kTreeNode != null)
                {
                    tvRootSel.Nodes.Add(kTreeNode);
                    kSelectedNode.EnsureVisible();

                    m_kTreeNode = kTreeNode;
                    m_kSelectedTreeNode = tvRootSel.SelectedNode = kSelectedNode;
                }
                else
                {
                    m_kSelectedTreeNode = m_kDefaultNode;
                }
            }
        }

		public void UpdateContents()
		{
            // Update model root text box contents.
            if (MFramework.Instance.Animation.ModelRoot == null ||
                MFramework.Instance.Animation.ModelRoot == string.Empty)
            {
                tbRoot.Text = "<UNDEFINED>";
            }
            else
            {
                tbRoot.Text = MFramework.Instance.Animation.ModelRoot;
            }

            // Update model root text box tool tip.
            string strToolTip = this.ttCharacterEdit.GetToolTip(
                this.tbRoot);
            char[] acSeparators = new char[] {'\n'};
            string[] astrLines = strToolTip.Split(acSeparators);
            this.ttCharacterEdit.SetToolTip(this.tbRoot,
                astrLines[0] + "\n" + this.tbRoot.Text);

            // Update model path text box contents.
            if (MFramework.Instance.Animation.ModelPath == null ||
                MFramework.Instance.Animation.ModelPath == string.Empty)
            {
                tbFile.Text = "<UNDEFINED>";
            }
            else
            {
                tbFile.Text = Path.GetFileName(MFramework.Instance.Animation
                    .FullModelPath);
            }

            // Update model path text box tool tip.
            strToolTip = this.ttCharacterEdit.GetToolTip(
                this.tbFile);
            astrLines = strToolTip.Split(acSeparators);
            this.ttCharacterEdit.SetToolTip(this.tbFile,
                astrLines[0] + "\n" +
                MFramework.Instance.Animation.FullModelPath);

            // Update edit root button active state.
            if (MFramework.Instance.Animation.FullModelPath != string.Empty)
            {
                btnEditRoot.Enabled = true;
            }
            else
            {
                btnEditRoot.Enabled = false;
            }

            // Tree View Update
            tvRootSel.Nodes.Clear();
            if (m_kDefaultNode != null)
            {
                tvRootSel.Nodes.Add(m_kDefaultNode);
            }

            if (m_kTreeNode != null)
            {
                tvRootSel.Nodes.Add(m_kTreeNode);

                if (m_kSelectedTreeNode != null)
                {
                    m_kSelectedTreeNode.EnsureVisible();
                    tvRootSel.SelectedNode = m_kSelectedTreeNode;
                }
            }

            // Parts List Update
            ListViewUtil.RefreshListWith_Parts(ref lvPartsList, ref m_kItemFolderSelected);
		}

        private TreeNode CreateNodesFromDirectory(ref System.IO.DirectoryInfo kDirInfo, ref TreeNode kSelectedNode)
        {
            System.IO.DirectoryInfo[] akDirInfo = kDirInfo.GetDirectories();

            // Hidden Directory는 추가하지 않는다.
            if((kDirInfo.Attributes & FileAttributes.Hidden) != 0)
            {
                return null;
            }

            // 서브-디렉토리가 없을 경우 그 디렉토리만 TreeNode에 추가한다.
            if (akDirInfo == null || akDirInfo.Length == 0)
            {
                TreeNode kNewNode = new TreeNode(kDirInfo.Name);

                // 루트 디렉토리인 경우 그것을 선택노드로 설정한다.
                if (kDirInfo.FullName == m_kItemFolderRootPath)
                {
                    kSelectedNode = kNewNode;
                }

                AnimationHelpers.ExtractItemXml(ref kDirInfo);

                return kNewNode;
            }

            TreeNode akRootNode = new TreeNode(kDirInfo.Name);
            AnimationHelpers.ExtractItemXml(ref kDirInfo);

            for(int i=0; i<akDirInfo.Length; ++i)
            {
                // 디렉토리 속성이 Hidden이 아닌 경우에만 노드를 추가한다.
                if ((akDirInfo[i].Attributes & FileAttributes.Hidden) == 0)
                {
                    TreeNode kTempNode = CreateNodesFromDirectory(ref akDirInfo[i], ref kSelectedNode);
                    if (kTempNode != null)
                    {
                        akRootNode.Nodes.Add(kTempNode);
                    }    
                }
            }

            // 루트 디렉토리인 경우 그것을 선택노드로 설정한다.
            if (kDirInfo.FullName == m_kItemFolderRootPath)
            {
                kSelectedNode = akRootNode;
            }

            return akRootNode;
        }
        
        private void btnEditRoot_Click(object sender, System.EventArgs e)
        {
            AnimationHelpers.DoModelRootDialog(this.ParentForm);
            UpdateContents();
        }

        private void lvPartsList_ColumnClick(object sender,
            System.Windows.Forms.ColumnClickEventArgs e)
        {
            ColumnSort.ListView_ColumnClick(sender, e, m_kColumnSorter);
        }

        private void lvPartsList_ItemCheck(object sender,
            System.Windows.Forms.ItemCheckEventArgs e)
        {
			MItem kPart = (MItem)lvPartsList.Items[e.Index].Tag;
			int iSelectedGender = cbSelectGender.SelectedIndex + 1;
			int iSelectedClass = cbSelectClass.SelectedIndex + 1;

			if (e.CurrentValue == CheckState.Unchecked)
			{
				if (e.NewValue == CheckState.Checked)
				{
					if (!kPart.Attached && !MFramework.Instance.Animation.AttachParts(kPart, iSelectedGender, iSelectedClass))
					{
						e.NewValue = CheckState.Unchecked;
					}
				}
			}
			else
			{
				if (e.NewValue == CheckState.Unchecked)
				{
					if (kPart.Attached && !MFramework.Instance.Animation.DetachParts(kPart, iSelectedGender, iSelectedClass))
					{
						e.NewValue = CheckState.Checked;
					}
				}
			}

            ListViewUtil.RefreshListWith_ItemCheck(ref lvPartsList);
        }

        private void tvRootSel_AfterSelect(object sender, TreeViewEventArgs e)
        {
            string strPath = e.Node.FullPath;
            int iPos = strPath.IndexOf("\\");

            if (e.Node.Equals(m_kDefaultNode))
            {
                m_kItemFolderSelected = "Default";
            }
            else
            {
                m_kItemFolderSelected = m_kItemFolderRootPath;
            }

            if (iPos > 0)
            {
                m_kItemFolderSelected = m_kItemFolderRootPath + "\\" + strPath.Substring(iPos+1); ;
            }

            m_kSelectedTreeNode = e.Node;
            ListViewUtil.RefreshListWith_Parts(ref lvPartsList, ref m_kItemFolderSelected);
        }


        #region ContextMenu Creation
        private void cmItemView_Popup(object sender, System.EventArgs e)
        {
            cmItemView.MenuItems.Clear();
            if (lvPartsList.SelectedItems.Count == 0)
            {
                cmItemView.MenuItems.AddRange(new System.Windows.Forms.MenuItem[]
                    {
                        miNewItem,
                        miRefreshThisFolder,
                    });
            }
            else
            {
                cmItemView.MenuItems.AddRange(new System.Windows.Forms.MenuItem[]
                    {
                        miNewItem,
                        miModifyItem,
                        miDeleteItem,
                        miRefreshItem,
                        miViewItemSceneGraph
                    });
            }
        }

        #endregion

        #region Menu Item Handlers

        private void miNewItem_Click(object sender, System.EventArgs e)
        {
			ItemForm kItemManager = new ItemForm();
			kItemManager.SetMode(ItemForm.EditMode.NEW);
			kItemManager.SetXmlPath(m_kItemFolderSelected);
			
			DialogResult kRet = kItemManager.ShowDialog();
			if (kRet == DialogResult.OK)
			{
				MFramework.Instance.Animation.AddToItemList(kItemManager.GetXmlFullPath(), kItemManager.GetXmlPath());
				UpdateContents();
			}
        }

        private void miModifyItem_Click(object sender, System.EventArgs e)
        {
            ListViewItem kLVItem = ListViewUtil.GetFirstSelectedItem(ref lvPartsList);
            if (kLVItem == null)
            {
                return;
            }

			MItem kItem = (MItem)kLVItem.Tag;

            ItemForm kItemManager = new ItemForm();
            kItemManager.SetMode(ItemForm.EditMode.MODIFY);
			kItemManager.SetTargetItem(kItem);
			
			DialogResult kRet = kItemManager.ShowDialog();
			if (kRet == DialogResult.OK)
			{
				MessageBox.Show("OK");
			}
			else if (kRet == DialogResult.Cancel)
			{
				MessageBox.Show("Cancel");
			}
        }

        private void miDeleteItem_Click(object sender, System.EventArgs e)
        {
			if (AnimationHelpers.OpenPartRemoveDialog(this.ParentForm, ref lvPartsList, m_kItemFolderSelected))
			{
				UpdateContents();
			}
        }

        private void miRefreshItem_Click(object sender, System.EventArgs e)
        {

        }

        private void miRefreshThisFolder_Click(object sender, System.EventArgs e)
        {

        }

        private void miViewItemSceneGraph_Click(object sender, System.EventArgs e)
        {

        }
        
        #endregion

		private void btnRandomGenerate_Click(object sender, EventArgs e)
		{
			if (MFramework.Instance.Animation.KFMFilename == null)
			{
				return;
			}

			int iSelectedGender = cbSelectGender.SelectedIndex + 1;
			int iSelectedClass = cbSelectClass.SelectedIndex + 1;

			Random kRandom = new Random();
			ArrayList akEquip = new ArrayList(10);
			ArrayList akVisitedNodes = new ArrayList(20);
			TreeNode kRootNode = m_kSelectedTreeNode;
			
			int iNbRootNodes = kRootNode.Nodes.Count;
			if (iNbRootNodes == 0)
			{
				return;
			}
					
			int iLoopCnt = 0;
			const int iMaxLoop = 50;
			while (++iLoopCnt < iMaxLoop)
			{
				bool bVisitedNode = false;
				TreeNode kChildNode = kRootNode.Nodes[kRandom.Next(iNbRootNodes)];

				while (kChildNode != null)
				{
					int iNbChildNode = kChildNode.Nodes.Count;
					if (iNbChildNode == 0)
					{
						// Leaf Node일 경우, 그 Folder안에 있는 아이템 중 하나를 선택
						if (akVisitedNodes.Contains(kChildNode))
						{
							bVisitedNode = true;
						}
						else
						{
							akVisitedNodes.Add(kChildNode);
						}
						break;
					}

					kChildNode = kChildNode.Nodes[kRandom.Next(iNbChildNode)];
				}

				if (bVisitedNode)
				{
					continue;
				}

				string kFolderPath = kChildNode.FullPath;
				int iPos = kFolderPath.IndexOf('\\');
				if (iPos > 0)
				{
					kFolderPath = m_kItemFolderRootPath + kFolderPath.Substring(iPos);
				}
				else if (kChildNode.Equals(m_kDefaultNode))
				{
					kFolderPath = "Default";
				}
				else
				{
					continue;
				}

				ArrayList akParts = MFramework.Instance.Animation.GetItemsByFolder(kFolderPath);
				if (akParts != null)
				{
					MItem kItem = (MItem)akParts[kRandom.Next(akParts.Count)];
					if (!akEquip.Contains(kItem.ItemPos))
					{
						if (MFramework.Instance.Animation.AttachParts(kItem, iSelectedGender, iSelectedClass))
						{
							akEquip.Add(kItem.ItemPos);
						}
						else
						{
							akVisitedNodes.Remove(kChildNode);
						}
					}
					
				}
			}

			ListViewUtil.RefreshListWith_ItemCheck(ref lvPartsList);
			MFramework.Instance.Input.SubmitCommand(MUICommand.CommandType.ZOOM_EXTENTS);
		}
    }
}

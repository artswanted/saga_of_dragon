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
using System.Threading;
using System.Windows.Forms;
using System.Diagnostics;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;

namespace Emergent.Gamebryo.SceneDesigner.GUI
{
	public class SelectionSetPanel : Form
	{
		#region Private Data
		private ISelectionSetService m_selectionSetService;
		private ISelectionService ms_pmSelectionService = null;
		private IUICommandService m_uiCommandService;
		private ServiceProvider m_serviceProvider;
		#endregion

		private System.Windows.Forms.ListBox m_lbSelectionSets;
		private System.Windows.Forms.Button m_btnAdd;
		private System.Windows.Forms.Button m_btnRename;
		private System.Windows.Forms.Button m_btnRemove;
		private System.Windows.Forms.Button m_btnSelect;
		private System.Windows.Forms.Label m_lblSelectionSets;
		private System.Windows.Forms.Label m_lblDummy;
		private Button m_btnAllCopy;
		private Button m_btnCopy;
		private System.ComponentModel.IContainer components = null;

		public SelectionSetPanel()
		{
			// This call is required by the Windows Form Designer.
			InitializeComponent();
			AttachToEventManager();
			UICommandService.BindCommands(this);
		}

		private ISelectionSetService SelectionSetService
		{
			get
			{
				if (m_selectionSetService == null)
				{
					
					m_selectionSetService = 
						ServiceProvider.GetService(
						typeof(ISelectionSetService)) as ISelectionSetService;
				}
				return m_selectionSetService;
			}
		}

		private IUICommandService UICommandService
		{
			get
			{
				if (m_uiCommandService == null)
				{
					m_uiCommandService = 
						ServiceProvider.GetService(
						typeof(IUICommandService))
						as IUICommandService;
				}
				return m_uiCommandService;
			}
		}

		private ServiceProvider ServiceProvider
		{
			get
			{
				if (m_serviceProvider == null)
				{
					m_serviceProvider = PluginAPI.ServiceProvider.Instance;
				}
				return m_serviceProvider;
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(SelectionSetPanel));
			this.m_lbSelectionSets = new System.Windows.Forms.ListBox();
			this.m_btnAdd = new System.Windows.Forms.Button();
			this.m_btnRename = new System.Windows.Forms.Button();
			this.m_btnRemove = new System.Windows.Forms.Button();
			this.m_btnSelect = new System.Windows.Forms.Button();
			this.m_lblSelectionSets = new System.Windows.Forms.Label();
			this.m_lblDummy = new System.Windows.Forms.Label();
			this.m_btnAllCopy = new System.Windows.Forms.Button();
			this.m_btnCopy = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// m_lbSelectionSets
			// 
			this.m_lbSelectionSets.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.m_lbSelectionSets.IntegralHeight = false;
			this.m_lbSelectionSets.ItemHeight = 12;
			this.m_lbSelectionSets.Location = new System.Drawing.Point(10, 34);
			this.m_lbSelectionSets.Name = "m_lbSelectionSets";
			this.m_lbSelectionSets.SelectionMode = System.Windows.Forms.SelectionMode.MultiExtended;
			this.m_lbSelectionSets.Size = new System.Drawing.Size(397, 281);
			this.m_lbSelectionSets.Sorted = true;
			this.m_lbSelectionSets.TabIndex = 1;
			this.m_lbSelectionSets.DoubleClick += new System.EventHandler(this.m_lbSelectionSets_DoubleClick);
			this.m_lbSelectionSets.KeyDown += new System.Windows.Forms.KeyEventHandler(this.m_lbSelectionSets_KeyDown);
			// 
			// m_btnAdd
			// 
			this.m_btnAdd.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
			this.m_btnAdd.FlatStyle = System.Windows.Forms.FlatStyle.System;
			this.m_btnAdd.Location = new System.Drawing.Point(77, 356);
			this.m_btnAdd.Name = "m_btnAdd";
			this.m_btnAdd.Size = new System.Drawing.Size(77, 24);
			this.m_btnAdd.TabIndex = 4;
			this.m_btnAdd.Text = "Add";
			this.m_btnAdd.Click += new System.EventHandler(this.m_btnAdd_Click);
			// 
			// m_btnRename
			// 
			this.m_btnRename.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
			this.m_btnRename.FlatStyle = System.Windows.Forms.FlatStyle.System;
			this.m_btnRename.Location = new System.Drawing.Point(163, 321);
			this.m_btnRename.Name = "m_btnRename";
			this.m_btnRename.Size = new System.Drawing.Size(77, 25);
			this.m_btnRename.TabIndex = 3;
			this.m_btnRename.Text = "Rename";
			this.m_btnRename.Click += new System.EventHandler(this.m_btnRename_Click);
			// 
			// m_btnRemove
			// 
			this.m_btnRemove.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
			this.m_btnRemove.FlatStyle = System.Windows.Forms.FlatStyle.System;
			this.m_btnRemove.Location = new System.Drawing.Point(163, 356);
			this.m_btnRemove.Name = "m_btnRemove";
			this.m_btnRemove.Size = new System.Drawing.Size(77, 24);
			this.m_btnRemove.TabIndex = 5;
			this.m_btnRemove.Text = "Remove";
			this.m_btnRemove.Click += new System.EventHandler(this.m_btnRemove_Click);
			// 
			// m_btnSelect
			// 
			this.m_btnSelect.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
			this.m_btnSelect.FlatStyle = System.Windows.Forms.FlatStyle.System;
			this.m_btnSelect.Location = new System.Drawing.Point(77, 321);
			this.m_btnSelect.Name = "m_btnSelect";
			this.m_btnSelect.Size = new System.Drawing.Size(77, 25);
			this.m_btnSelect.TabIndex = 2;
			this.m_btnSelect.Text = "Select";
			this.m_btnSelect.Click += new System.EventHandler(this.m_btnSelect_Click);
			// 
			// m_lblSelectionSets
			// 
			this.m_lblSelectionSets.AutoSize = true;
			this.m_lblSelectionSets.Location = new System.Drawing.Point(10, 9);
			this.m_lblSelectionSets.Name = "m_lblSelectionSets";
			this.m_lblSelectionSets.Size = new System.Drawing.Size(90, 12);
			this.m_lblSelectionSets.TabIndex = 0;
			this.m_lblSelectionSets.Text = "Selection Sets:";
			// 
			// m_lblDummy
			// 
			this.m_lblDummy.Location = new System.Drawing.Point(173, 164);
			this.m_lblDummy.Name = "m_lblDummy";
			this.m_lblDummy.Size = new System.Drawing.Size(9, 8);
			this.m_lblDummy.TabIndex = 6;
			// 
			// m_btnAllCopy
			// 
			this.m_btnAllCopy.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
			this.m_btnAllCopy.FlatStyle = System.Windows.Forms.FlatStyle.System;
			this.m_btnAllCopy.Location = new System.Drawing.Point(246, 356);
			this.m_btnAllCopy.Name = "m_btnAllCopy";
			this.m_btnAllCopy.Size = new System.Drawing.Size(77, 24);
			this.m_btnAllCopy.TabIndex = 4;
			this.m_btnAllCopy.Text = "All Copy";
			this.m_btnAllCopy.Click += new System.EventHandler(this.m_btnAllCopy_Click);
			// 
			// m_btnCopy
			// 
			this.m_btnCopy.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
			this.m_btnCopy.FlatStyle = System.Windows.Forms.FlatStyle.System;
			this.m_btnCopy.Location = new System.Drawing.Point(246, 321);
			this.m_btnCopy.Name = "m_btnCopy";
			this.m_btnCopy.Size = new System.Drawing.Size(77, 24);
			this.m_btnCopy.TabIndex = 4;
			this.m_btnCopy.Text = "Copy";
			this.m_btnCopy.Click += new System.EventHandler(this.m_btnCopy_Click);
			// 
			// SelectionSetPanel
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(6, 14);
			this.AutoScroll = true;
			this.ClientSize = new System.Drawing.Size(416, 390);
			this.Controls.Add(this.m_btnSelect);
			this.Controls.Add(this.m_btnCopy);
			this.Controls.Add(this.m_btnAllCopy);
			this.Controls.Add(this.m_btnAdd);
			this.Controls.Add(this.m_btnRename);
			this.Controls.Add(this.m_btnRemove);
			this.Controls.Add(this.m_lblSelectionSets);
			this.Controls.Add(this.m_lbSelectionSets);
			this.Controls.Add(this.m_lblDummy);
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Name = "SelectionSetPanel";
			this.Text = "Selection Sets";
			this.Load += new System.EventHandler(this.SelectionSetPanel_Load);
			this.ResumeLayout(false);
			this.PerformLayout();

		}
		#endregion

		#region EventManager Handlers

		private void EventManager_NewSceneLoaded(MScene pmScene)
		{
			SyncListBoxToSelectionManager();
		}

		private void EventManager_SelectionSetRemovedFromScene(MScene pmScene,
			MSelectionSet pmSelectionSet)
		{
			SyncListBoxToSelectionManager();
		}

		private void EventManager_SelectionSetAddedToScene(MScene pmScene, 
			MSelectionSet pmSelectionSet)
		{
			SyncListBoxToSelectionManager();
		}

		private void EventManager_SelectionSetNameChanged(
			MSelectionSet pmSelectionSet, string strOldName, bool bInBatch)
		{
			MFramework.Instance.Scene.Dirty = true;
			SyncListBoxToSelectionManager();
		}

		#endregion

		#region Private Helpers


		private ISelectionService SelectionService
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

		private void AttachToEventManager()
		{
			MFramework fw = MFramework.Instance;
			fw.EventManager.SelectionSetAddedToScene +=
				new MEventManager.__Delegate_SelectionSetAddedToScene(
				EventManager_SelectionSetAddedToScene);
			fw.EventManager.SelectionSetRemovedFromScene +=
				new MEventManager.__Delegate_SelectionSetRemovedFromScene(
				EventManager_SelectionSetRemovedFromScene);
			fw.EventManager.SelectionSetNameChanged +=
				new MEventManager.__Delegate_SelectionSetNameChanged(
				EventManager_SelectionSetNameChanged);
			fw.EventManager.NewSceneLoaded += 
				new MEventManager.__Delegate_NewSceneLoaded(
				EventManager_NewSceneLoaded);
		}

		private void SyncListBoxToSelectionManager()
		{		  
			m_lbSelectionSets.Items.Clear();
			MSelectionSet[] selectionSets = 
				SelectionSetService.GetSelectionSets();
			foreach (MSelectionSet selectionSet in selectionSets)
			{
				m_lbSelectionSets.Items.Add(selectionSet.Name);
			}
		}
		
		#endregion

		#region WinForms event handlers

		private void SelectionSetPanel_Load(object sender, System.EventArgs e)
		{
			SyncListBoxToSelectionManager();
		}

		private void m_btnSelect_Click(object sender, System.EventArgs e)
		{
			SelectionService.ClearSelectedEntities();
			ListBox.SelectedIndexCollection indeces = 
				m_lbSelectionSets.SelectedIndices;
			foreach (int index in indeces)
			{
				string name = m_lbSelectionSets.GetItemText(
					m_lbSelectionSets.Items[index]);
				MSelectionSet selectionSet = 
					SelectionSetService.GetSelectionSetByName(name);
				SelectionService.AddSelectionSetToSelection(selectionSet);
			}
			m_lbSelectionSets.SelectedIndex = -1;
		}

		private void m_btnRename_Click(object sender, System.EventArgs e)
		{
			SelectionSetNameDlg dlg = new SelectionSetNameDlg();
			dlg.Text = "Rename Selection Set";
			string name = m_lbSelectionSets.SelectedItem as string;
			dlg.NewName = name;
			if (dlg.ShowDialog() == DialogResult.OK)
			{
				string newName = dlg.NewName;
				MSelectionSet selectionSet =
					SelectionSetService.GetSelectionSetByName(name);
				if (!name.Equals(newName))
				{
					MSelectionSet existingSet = 
						SelectionSetService.GetSelectionSetByName(newName);
					if (existingSet == null)
					{
						selectionSet.Name = dlg.NewName;
					}
					else
					{
						MessageBox.Show("Selection Set "
							+ newName + " Already Exists.");
					}
				}
			}		
		}
		
		private void m_btnAdd_Click(object sender, System.EventArgs e)
		{
			SelectionSetNameDlg dlg = new SelectionSetNameDlg();
			dlg.Text = "Add New Selection Set";
			dlg.NewName = MFramework.Instance.Scene.GetUniqueSelectionSetName("Selection Set 01");
			if (dlg.ShowDialog() == DialogResult.OK)
			{				
				MSelectionSet preexistingSelectionSet = SelectionSetService.GetSelectionSetByName(dlg.NewName);
					
				if (preexistingSelectionSet != null)
				{
					if (MessageBox.Show(
						"A selection set with that name already exists\n" +
						" would you like to replace it?", "Confirm", 
						MessageBoxButtons.OKCancel) ==
						DialogResult.OK)
					{
						SelectionSetService.RemoveSelectionSet(
							preexistingSelectionSet);
					}
					else
					{
						return;
					}

				}

				MSelectionSet selectionSet = MSelectionSet.Create(dlg.NewName);
				MEntity[] selectedEntities = SelectionService.GetSelectedEntities();
				foreach(MEntity entity in selectedEntities)
				{
					selectionSet.AddEntity(entity);
				}

				SelectionSetService.AddSelectionSet(selectionSet);
			}
		}

		private void m_btnRemove_Click(object sender, System.EventArgs e)
		{
			ListBox.SelectedIndexCollection indeces = m_lbSelectionSets.SelectedIndices;
			ArrayList removalList = new ArrayList();
			foreach (int index in indeces)
			{
				string name = m_lbSelectionSets.GetItemText(m_lbSelectionSets.Items[index]);
				removalList.Add(name);
			}
			foreach (string name in removalList)
			{
				MSelectionSet selectionSet = SelectionSetService.GetSelectionSetByName(name);
				SelectionSetService.RemoveSelectionSet(selectionSet);
				//SelectionService.AddToSelection(selectionSet);				
			}
		}

		//
		// HandOver, SelectionSet Copy, 강정욱 2008.01.31
		//
		private void CopySelectionSet(bool bCopyEntities)
		{
			// 선택한 Selection Set의 Entity들을 복사해서
			// 새로운 Selection Set을 만든다.
			ArrayList pmNewSelectionSets = new ArrayList();

			// 새로 카피 하기 위해서 선택한것들을 저장 해둔다.
			// 그리고 Entity를 카피해야 할 경우에 Entity도 카피 한다.
			SelectionService.ClearSelectedEntities();
			ListBox.SelectedIndexCollection indeces = m_lbSelectionSets.SelectedIndices;
			foreach (int index in indeces)
			{
				string kName = m_lbSelectionSets.GetItemText(m_lbSelectionSets.Items[index]);
				MSelectionSet selectionSet = SelectionSetService.GetSelectionSetByName(kName);

				// New SelectionSet 구성.
				MSelectionSet kNewSet = MSelectionSet.Create(kName);
				MEntity[] entities = selectionSet.GetEntities();
				foreach (MEntity entity in entities)
				{
					MEntity kEntityForAdd = entity;
					if (bCopyEntities)
					{
						// Entity도 카피한다.
						string newName = MFramework.Instance.Scene.GetUniqueEntityName(entity.Name);
						kEntityForAdd = entity.Clone(newName, false);
						MFramework.Instance.Scene.AddEntity(kEntityForAdd, true);
					}
					kNewSet.AddEntity(kEntityForAdd);
				}
				pmNewSelectionSets.Add(kNewSet);
			}

			// Set 진짜로 카피 해준다.
			foreach (MSelectionSet kNewSet in pmNewSelectionSets)
			{
				string kNewName = kNewSet.Name;
				//string kNewName = "Copy_" + kNewSet.Name;
				//kNewSet.Name = MFramework.Instance.Scene.GetUniqueSelectionSetName(kNewName);
				kNewSet.Name = MFramework.Instance.Scene.GetUniqueSelectionSetName(kNewName);
				SelectionSetService.AddSelectionSet(kNewSet);
			}

			// 카피 한것들을 선택해 준다.
			foreach (MSelectionSet kNewSet in pmNewSelectionSets)
			{
				int iIdx = m_lbSelectionSets.FindString(kNewSet.Name, 0);
				m_lbSelectionSets.SelectedIndex = iIdx;
			}
		}

		private void m_btnCopy_Click(object sender, EventArgs e)
		{
			CopySelectionSet(false);
		}

		private void m_btnAllCopy_Click(object sender, EventArgs e)
		{
			CopySelectionSet(true);
		}
		

		private void m_lbSelectionSets_KeyDown(object sender, System.Windows.Forms.KeyEventArgs e)
		{
			switch (e.KeyData)
			{
				case Keys.Delete:
				{
					if (m_btnRemove.Enabled)
					{
						m_btnRemove_Click(sender, e);
					}
					break;
				}
			}
		}

		private void m_lbSelectionSets_DoubleClick(object sender,
			System.EventArgs e)
		{
			m_btnSelect_Click(sender, e);
		}


		[UICommandHandler("Idle")]
		private void Application_Idle(object sender, EventArgs e)
		{
			//Determine which Buttons should be enabled
			//Select Button, there must be one or more items selected 
			//in the list box
			ListBox.SelectedObjectCollection selectListBoxItems = 
				m_lbSelectionSets.SelectedItems;
			m_btnSelect.Enabled = selectListBoxItems.Count > 0;
			//Rename Button - one and only one listbox itme must be selected
			m_btnRename.Enabled = selectListBoxItems.Count == 1;
			//Add button - there must be one ore more entities selected
			MEntity[] selectedEntities = 
				SelectionService.GetSelectedEntities();
			m_btnAdd.Enabled = selectedEntities.Length > 0;
			//Remove Button - there must be one or more items selected 
			//in the list box
			m_btnRemove.Enabled = selectListBoxItems.Count > 0;

		}

		#endregion
	}
}


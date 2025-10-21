using System.Collections;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;
using Emergent.Gamebryo.SceneDesigner.Framework;

namespace Emergent.Gamebryo.SceneDesigner.GUI.ProjectG
{
	partial class MergePanel
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			System.Windows.Forms.CheckBox m_cbGetByNewSelectionSet;
			this.m_lbEntityList = new System.Windows.Forms.ListBox();
			this.m_btnMerge = new System.Windows.Forms.Button();
			this.m_btnCancel = new System.Windows.Forms.Button();
			this.m_lbSelectionSetList = new System.Windows.Forms.ListBox();
			this.m_lblEntryList = new System.Windows.Forms.Label();
			this.m_lblSelectionSetList = new System.Windows.Forms.Label();
			this.m_tbNewSelectionSet = new System.Windows.Forms.TextBox();
			m_cbGetByNewSelectionSet = new System.Windows.Forms.CheckBox();
			this.SuspendLayout();
			// 
			// m_cbGetByNewSelectionSet
			// 
			m_cbGetByNewSelectionSet.AutoSize = true;
			m_cbGetByNewSelectionSet.Location = new System.Drawing.Point(15, 373);
			m_cbGetByNewSelectionSet.Name = "m_cbGetByNewSelectionSet";
			m_cbGetByNewSelectionSet.Size = new System.Drawing.Size(134, 16);
			m_cbGetByNewSelectionSet.TabIndex = 7;
			m_cbGetByNewSelectionSet.Text = "새 Set으로 가져오기";
			m_cbGetByNewSelectionSet.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			m_cbGetByNewSelectionSet.UseVisualStyleBackColor = true;
			m_cbGetByNewSelectionSet.CheckedChanged += new System.EventHandler(this.m_cbGetByNewSelectionSet_CheckedChanged);
			// 
			// m_lbEntityList
			// 
			this.m_lbEntityList.FormattingEnabled = true;
			this.m_lbEntityList.ItemHeight = 12;
			this.m_lbEntityList.Location = new System.Drawing.Point(13, 25);
			this.m_lbEntityList.Name = "m_lbEntityList";
			this.m_lbEntityList.SelectionMode = System.Windows.Forms.SelectionMode.MultiExtended;
			this.m_lbEntityList.Size = new System.Drawing.Size(346, 340);
			this.m_lbEntityList.TabIndex = 1;
			this.m_lbEntityList.SelectedIndexChanged += new System.EventHandler(this.m_lbEntityList_SelectedIndexChanged);
			// 
			// m_btnMerge
			// 
			this.m_btnMerge.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnMerge.Location = new System.Drawing.Point(380, 371);
			this.m_btnMerge.Name = "m_btnMerge";
			this.m_btnMerge.Size = new System.Drawing.Size(112, 23);
			this.m_btnMerge.TabIndex = 2;
			this.m_btnMerge.Text = "가져오기";
			this.m_btnMerge.UseVisualStyleBackColor = true;
			this.m_btnMerge.Click += new System.EventHandler(this.m_btnMerge_Click);
			// 
			// m_btnCancel
			// 
			this.m_btnCancel.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnCancel.Location = new System.Drawing.Point(498, 371);
			this.m_btnCancel.Name = "m_btnCancel";
			this.m_btnCancel.Size = new System.Drawing.Size(112, 23);
			this.m_btnCancel.TabIndex = 3;
			this.m_btnCancel.Text = "취   소";
			this.m_btnCancel.UseVisualStyleBackColor = true;
			this.m_btnCancel.Click += new System.EventHandler(this.m_btnCancel_Click);
			// 
			// m_lbSelectionSetList
			// 
			this.m_lbSelectionSetList.FormattingEnabled = true;
			this.m_lbSelectionSetList.ItemHeight = 12;
			this.m_lbSelectionSetList.Location = new System.Drawing.Point(380, 25);
			this.m_lbSelectionSetList.Name = "m_lbSelectionSetList";
			this.m_lbSelectionSetList.SelectionMode = System.Windows.Forms.SelectionMode.MultiExtended;
			this.m_lbSelectionSetList.Size = new System.Drawing.Size(230, 340);
			this.m_lbSelectionSetList.TabIndex = 4;
			this.m_lbSelectionSetList.SelectedIndexChanged += new System.EventHandler(this.m_lbSelectionSetList_SelectedIndexChanged);
			// 
			// m_lblEntryList
			// 
			this.m_lblEntryList.AutoSize = true;
			this.m_lblEntryList.Location = new System.Drawing.Point(13, 7);
			this.m_lblEntryList.Name = "m_lblEntryList";
			this.m_lblEntryList.Size = new System.Drawing.Size(54, 12);
			this.m_lblEntryList.TabIndex = 5;
			this.m_lblEntryList.Text = "Entry list";
			// 
			// m_lblSelectionSetList
			// 
			this.m_lblSelectionSetList.AutoSize = true;
			this.m_lblSelectionSetList.Location = new System.Drawing.Point(378, 7);
			this.m_lblSelectionSetList.Name = "m_lblSelectionSetList";
			this.m_lblSelectionSetList.Size = new System.Drawing.Size(95, 12);
			this.m_lblSelectionSetList.TabIndex = 6;
			this.m_lblSelectionSetList.Text = "SelectionSet list";
			// 
			// m_tbNewSelectionSet
			// 
			this.m_tbNewSelectionSet.Location = new System.Drawing.Point(161, 371);
			this.m_tbNewSelectionSet.Name = "m_tbNewSelectionSet";
			this.m_tbNewSelectionSet.Size = new System.Drawing.Size(198, 21);
			this.m_tbNewSelectionSet.TabIndex = 8;
			this.m_tbNewSelectionSet.Text = "새 Set 이름";
			// 
			// MergePanel
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(624, 406);
			this.Controls.Add(this.m_tbNewSelectionSet);
			this.Controls.Add(m_cbGetByNewSelectionSet);
			this.Controls.Add(this.m_lblSelectionSetList);
			this.Controls.Add(this.m_lblEntryList);
			this.Controls.Add(this.m_lbSelectionSetList);
			this.Controls.Add(this.m_btnCancel);
			this.Controls.Add(this.m_btnMerge);
			this.Controls.Add(this.m_lbEntityList);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
			this.Name = "MergePanel";
			this.Text = "MergePanel";
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.ListBox m_lbEntityList;
		private System.Windows.Forms.Button m_btnMerge;
		private System.Windows.Forms.Button m_btnCancel;
		private System.Windows.Forms.ListBox m_lbSelectionSetList;
		private System.Windows.Forms.Label m_lblEntryList;
		private System.Windows.Forms.Label m_lblSelectionSetList;

		private bool m_bGetByNewSelSetName = false;
		private int m_iSectectedEntityCount = 0;
		private MScene m_pmScene;
		private string m_strGsaFilePath = null;
		private ArrayList m_pmEntityList = new ArrayList();
		private ArrayList m_pmSelectionSetList = new ArrayList();
		private EntityComparer m_pmEntityComparer = new EntityComparer();
		private SelectionSetComparer m_pmSelectionSetComparer = new SelectionSetComparer();
		
		private enum FilterState
		{
			Visible,
			Hidden,
			Frozen
		};
		
		private FilterState m_eFilterState = FilterState.Visible;
		private System.Windows.Forms.TextBox m_tbNewSelectionSet;

	}
}
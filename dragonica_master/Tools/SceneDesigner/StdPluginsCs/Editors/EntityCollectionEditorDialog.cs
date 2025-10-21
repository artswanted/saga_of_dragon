using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Diagnostics;
using Emergent.Gamebryo.SceneDesigner.Framework;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Editors
{
	/// <summary>
	/// Summary description for EntityAddRemoveDialog.
	/// </summary>
	public class EntityCollectionEditorDialog : System.Windows.Forms.Form
	{
        private System.Windows.Forms.ListBox m_lbEntitiesInScene;
        private System.Windows.Forms.Label m_lblEntitiesInScene;
        private System.Windows.Forms.Button m_btnAdd;
        private System.Windows.Forms.Button m_btnRemove;
        private System.Windows.Forms.Button m_btnOK;
        private System.Windows.Forms.Button m_btnCancel;
        private System.Windows.Forms.Label m_lblEntitiesInCollection;
        private System.Windows.Forms.ListBox m_lbEntitiesInCollection;
		private TextBox m_tbEntitiesInScene;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;
		private TextBox m_tbEntitiesInCollection;
		private Button m_btnSelectAllEIS;
		private Button m_btnSelectRevertEIS;
		private Button m_btnSelectAllEIC;
		private Button m_btnSelectRevertEIC;
		private Button m_btnCopyScene;
		private Button m_btnCollection;
		private bool m_bTextChangeCase = false;

		public EntityCollectionEditorDialog(MEntity[] amEntityCollection,
            MPropertyContainer pmSourceEntity)
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

            if (amEntityCollection != null)
            {
                foreach (MEntity pmEntity in amEntityCollection)
                {
                    m_pmEntityCollection.Add(pmEntity);
                }
            }
		    m_pmSourcePropertyContainer = pmSourceEntity;
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(EntityCollectionEditorDialog));
			this.m_lbEntitiesInScene = new System.Windows.Forms.ListBox();
			this.m_lblEntitiesInScene = new System.Windows.Forms.Label();
			this.m_lblEntitiesInCollection = new System.Windows.Forms.Label();
			this.m_btnAdd = new System.Windows.Forms.Button();
			this.m_btnRemove = new System.Windows.Forms.Button();
			this.m_btnOK = new System.Windows.Forms.Button();
			this.m_btnCancel = new System.Windows.Forms.Button();
			this.m_lbEntitiesInCollection = new System.Windows.Forms.ListBox();
			this.m_tbEntitiesInScene = new System.Windows.Forms.TextBox();
			this.m_tbEntitiesInCollection = new System.Windows.Forms.TextBox();
			this.m_btnSelectAllEIS = new System.Windows.Forms.Button();
			this.m_btnSelectRevertEIS = new System.Windows.Forms.Button();
			this.m_btnSelectAllEIC = new System.Windows.Forms.Button();
			this.m_btnSelectRevertEIC = new System.Windows.Forms.Button();
			this.m_btnCopyScene = new System.Windows.Forms.Button();
			this.m_btnCollection = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// m_lbEntitiesInScene
			// 
			this.m_lbEntitiesInScene.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)));
			this.m_lbEntitiesInScene.IntegralHeight = false;
			this.m_lbEntitiesInScene.ItemHeight = 12;
			this.m_lbEntitiesInScene.Location = new System.Drawing.Point(10, 56);
			this.m_lbEntitiesInScene.Name = "m_lbEntitiesInScene";
			this.m_lbEntitiesInScene.SelectionMode = System.Windows.Forms.SelectionMode.MultiExtended;
			this.m_lbEntitiesInScene.Size = new System.Drawing.Size(268, 362);
			this.m_lbEntitiesInScene.Sorted = true;
			this.m_lbEntitiesInScene.TabIndex = 3;
			this.m_lbEntitiesInScene.DoubleClick += new System.EventHandler(this.m_lbEntitiesInScene_DoubleClick);
			this.m_lbEntitiesInScene.SelectedIndexChanged += new System.EventHandler(this.m_lbEntitiesInScene_SelectedIndexChanged);
			// 
			// m_lblEntitiesInScene
			// 
			this.m_lblEntitiesInScene.AutoSize = true;
			this.m_lblEntitiesInScene.Location = new System.Drawing.Point(10, 9);
			this.m_lblEntitiesInScene.Name = "m_lblEntitiesInScene";
			this.m_lblEntitiesInScene.Size = new System.Drawing.Size(104, 12);
			this.m_lblEntitiesInScene.TabIndex = 0;
			this.m_lblEntitiesInScene.Text = "Entities in Scene:";
			// 
			// m_lblEntitiesInCollection
			// 
			this.m_lblEntitiesInCollection.AutoSize = true;
			this.m_lblEntitiesInCollection.Location = new System.Drawing.Point(384, 9);
			this.m_lblEntitiesInCollection.Name = "m_lblEntitiesInCollection";
			this.m_lblEntitiesInCollection.Size = new System.Drawing.Size(124, 12);
			this.m_lblEntitiesInCollection.TabIndex = 3;
			this.m_lblEntitiesInCollection.Text = "Entities in Collection:";
			// 
			// m_btnAdd
			// 
			this.m_btnAdd.Anchor = System.Windows.Forms.AnchorStyles.Left;
			this.m_btnAdd.FlatStyle = System.Windows.Forms.FlatStyle.System;
			this.m_btnAdd.Location = new System.Drawing.Point(289, 216);
			this.m_btnAdd.Name = "m_btnAdd";
			this.m_btnAdd.Size = new System.Drawing.Size(87, 25);
			this.m_btnAdd.TabIndex = 5;
			this.m_btnAdd.Text = "Add -->";
			this.m_btnAdd.Click += new System.EventHandler(this.m_btnAdd_Click);
			// 
			// m_btnRemove
			// 
			this.m_btnRemove.Anchor = System.Windows.Forms.AnchorStyles.Left;
			this.m_btnRemove.FlatStyle = System.Windows.Forms.FlatStyle.System;
			this.m_btnRemove.Location = new System.Drawing.Point(289, 260);
			this.m_btnRemove.Name = "m_btnRemove";
			this.m_btnRemove.Size = new System.Drawing.Size(87, 25);
			this.m_btnRemove.TabIndex = 6;
			this.m_btnRemove.Text = "<-- Remove";
			this.m_btnRemove.Click += new System.EventHandler(this.m_btnRemove_Click);
			// 
			// m_btnOK
			// 
			this.m_btnOK.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
			this.m_btnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.m_btnOK.FlatStyle = System.Windows.Forms.FlatStyle.System;
			this.m_btnOK.Location = new System.Drawing.Point(232, 467);
			this.m_btnOK.Name = "m_btnOK";
			this.m_btnOK.Size = new System.Drawing.Size(90, 25);
			this.m_btnOK.TabIndex = 11;
			this.m_btnOK.Text = "OK";
			// 
			// m_btnCancel
			// 
			this.m_btnCancel.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
			this.m_btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.m_btnCancel.FlatStyle = System.Windows.Forms.FlatStyle.System;
			this.m_btnCancel.Location = new System.Drawing.Point(348, 467);
			this.m_btnCancel.Name = "m_btnCancel";
			this.m_btnCancel.Size = new System.Drawing.Size(90, 25);
			this.m_btnCancel.TabIndex = 12;
			this.m_btnCancel.Text = "Cancel";
			// 
			// m_lbEntitiesInCollection
			// 
			this.m_lbEntitiesInCollection.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)));
			this.m_lbEntitiesInCollection.IntegralHeight = false;
			this.m_lbEntitiesInCollection.ItemHeight = 12;
			this.m_lbEntitiesInCollection.Location = new System.Drawing.Point(384, 56);
			this.m_lbEntitiesInCollection.Name = "m_lbEntitiesInCollection";
			this.m_lbEntitiesInCollection.SelectionMode = System.Windows.Forms.SelectionMode.MultiExtended;
			this.m_lbEntitiesInCollection.Size = new System.Drawing.Size(268, 362);
			this.m_lbEntitiesInCollection.Sorted = true;
			this.m_lbEntitiesInCollection.TabIndex = 4;
			this.m_lbEntitiesInCollection.DoubleClick += new System.EventHandler(this.m_lbEntitiesInCollection_DoubleClick);
			this.m_lbEntitiesInCollection.SelectedIndexChanged += new System.EventHandler(this.m_lbEntitiesInCollection_SelectedIndexChanged);
			// 
			// m_tbEntitiesInScene
			// 
			this.m_tbEntitiesInScene.Location = new System.Drawing.Point(10, 29);
			this.m_tbEntitiesInScene.Multiline = true;
			this.m_tbEntitiesInScene.Name = "m_tbEntitiesInScene";
			this.m_tbEntitiesInScene.Size = new System.Drawing.Size(210, 21);
			this.m_tbEntitiesInScene.TabIndex = 1;
			this.m_tbEntitiesInScene.TextChanged += new System.EventHandler(this.m_tbEntitiesInScene_TextChanged);
			// 
			// m_tbEntitiesInCollection
			// 
			this.m_tbEntitiesInCollection.Location = new System.Drawing.Point(384, 29);
			this.m_tbEntitiesInCollection.Multiline = true;
			this.m_tbEntitiesInCollection.Name = "m_tbEntitiesInCollection";
			this.m_tbEntitiesInCollection.Size = new System.Drawing.Size(210, 21);
			this.m_tbEntitiesInCollection.TabIndex = 2;
			this.m_tbEntitiesInCollection.TextChanged += new System.EventHandler(this.m_tbEntitiesInCollection_TextChanged);
			// 
			// m_btnSelectAllEIS
			// 
			this.m_btnSelectAllEIS.Location = new System.Drawing.Point(50, 424);
			this.m_btnSelectAllEIS.Name = "m_btnSelectAllEIS";
			this.m_btnSelectAllEIS.Size = new System.Drawing.Size(75, 23);
			this.m_btnSelectAllEIS.TabIndex = 7;
			this.m_btnSelectAllEIS.Text = "Select All";
			this.m_btnSelectAllEIS.UseVisualStyleBackColor = true;
			this.m_btnSelectAllEIS.Click += new System.EventHandler(this.m_btnSelectAllEIS_Click);
			// 
			// m_btnSelectRevertEIS
			// 
			this.m_btnSelectRevertEIS.Location = new System.Drawing.Point(131, 424);
			this.m_btnSelectRevertEIS.Name = "m_btnSelectRevertEIS";
			this.m_btnSelectRevertEIS.Size = new System.Drawing.Size(103, 23);
			this.m_btnSelectRevertEIS.TabIndex = 8;
			this.m_btnSelectRevertEIS.Text = "Select Revert";
			this.m_btnSelectRevertEIS.UseVisualStyleBackColor = true;
			this.m_btnSelectRevertEIS.Click += new System.EventHandler(this.m_btnSelectRevertEIS_Click);
			// 
			// m_btnSelectAllEIC
			// 
			this.m_btnSelectAllEIC.Location = new System.Drawing.Point(433, 424);
			this.m_btnSelectAllEIC.Name = "m_btnSelectAllEIC";
			this.m_btnSelectAllEIC.Size = new System.Drawing.Size(75, 23);
			this.m_btnSelectAllEIC.TabIndex = 9;
			this.m_btnSelectAllEIC.Text = "Select All";
			this.m_btnSelectAllEIC.UseVisualStyleBackColor = true;
			this.m_btnSelectAllEIC.Click += new System.EventHandler(this.m_btnSelectAllEIC_Click);
			// 
			// m_btnSelectRevertEIC
			// 
			this.m_btnSelectRevertEIC.Location = new System.Drawing.Point(514, 424);
			this.m_btnSelectRevertEIC.Name = "m_btnSelectRevertEIC";
			this.m_btnSelectRevertEIC.Size = new System.Drawing.Size(103, 23);
			this.m_btnSelectRevertEIC.TabIndex = 10;
			this.m_btnSelectRevertEIC.Text = "Select Revert";
			this.m_btnSelectRevertEIC.UseVisualStyleBackColor = true;
			this.m_btnSelectRevertEIC.Click += new System.EventHandler(this.m_btnSelectRevertEIC_Click);
			// 
			// m_btnCopyScene
			// 
			this.m_btnCopyScene.Location = new System.Drawing.Point(226, 29);
			this.m_btnCopyScene.Name = "m_btnCopyScene";
			this.m_btnCopyScene.Size = new System.Drawing.Size(52, 23);
			this.m_btnCopyScene.TabIndex = 13;
			this.m_btnCopyScene.Text = "Copy";
			this.m_btnCopyScene.UseVisualStyleBackColor = true;
			this.m_btnCopyScene.Click += new System.EventHandler(this.m_btnCopyScene_Click);
			// 
			// m_btnCollection
			// 
			this.m_btnCollection.Location = new System.Drawing.Point(600, 29);
			this.m_btnCollection.Name = "m_btnCollection";
			this.m_btnCollection.Size = new System.Drawing.Size(52, 23);
			this.m_btnCollection.TabIndex = 13;
			this.m_btnCollection.Text = "Copy";
			this.m_btnCollection.UseVisualStyleBackColor = true;
			this.m_btnCollection.Click += new System.EventHandler(this.m_btnCollection_Click);
			// 
			// EntityCollectionEditorDialog
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(6, 14);
			this.ClientSize = new System.Drawing.Size(669, 500);
			this.Controls.Add(this.m_btnCollection);
			this.Controls.Add(this.m_btnCopyScene);
			this.Controls.Add(this.m_btnSelectRevertEIC);
			this.Controls.Add(this.m_btnSelectRevertEIS);
			this.Controls.Add(this.m_btnSelectAllEIC);
			this.Controls.Add(this.m_btnSelectAllEIS);
			this.Controls.Add(this.m_tbEntitiesInCollection);
			this.Controls.Add(this.m_tbEntitiesInScene);
			this.Controls.Add(this.m_lbEntitiesInCollection);
			this.Controls.Add(this.m_lbEntitiesInScene);
			this.Controls.Add(this.m_btnCancel);
			this.Controls.Add(this.m_btnOK);
			this.Controls.Add(this.m_btnRemove);
			this.Controls.Add(this.m_btnAdd);
			this.Controls.Add(this.m_lblEntitiesInCollection);
			this.Controls.Add(this.m_lblEntitiesInScene);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "EntityCollectionEditorDialog";
			this.ShowInTaskbar = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "Entity Collection Editor";
			this.Load += new System.EventHandler(this.EntityAddRemoveDialog_Load);
			this.ResumeLayout(false);
			this.PerformLayout();

        }
		#endregion

        private ArrayList m_pmEntityCollection = new ArrayList();
        private MPropertyContainer m_pmSourcePropertyContainer;

        public MEntity[] GetEntityCollection()
        {
            return (MEntity[]) m_pmEntityCollection.ToArray(typeof(MEntity));
        }

        private void EntityAddRemoveDialog_Load(object sender,
            System.EventArgs e)
        {
            PopulateSceneListBox();
            PopulateCollectionListBox();
            UpdateButtonStates();
        }

        private void PopulateSceneListBox()
        {
            ListBox.SelectedObjectCollection pmSelectedItems =
                m_lbEntitiesInScene.SelectedItems;

            m_lbEntitiesInScene.Items.Clear();

            MEntity[] amSceneEntities = MFramework.Instance.Scene
                .GetEntities();
            foreach (MEntity pmEntity in amSceneEntities)
            {
                if (!m_pmEntityCollection.Contains(pmEntity) &&
                    m_pmSourcePropertyContainer != pmEntity)
                {
                    m_lbEntitiesInScene.Items.Add(pmEntity);
                }
            }

            foreach (object pmObject in pmSelectedItems)
            {
                int iIndex = m_lbEntitiesInScene.Items.IndexOf(pmObject);
                if (iIndex > 0)
                {
                    m_lbEntitiesInScene.SetSelected(iIndex, true);
                }
            }
            if (m_lbEntitiesInScene.SelectedIndex == -1 &&
                m_lbEntitiesInScene.Items.Count > 0)
            {
                m_lbEntitiesInScene.SelectedIndex = 0;
            }
        }

        private void PopulateCollectionListBox()
        {
            ListBox.SelectedObjectCollection pmSelectedItems =
                m_lbEntitiesInCollection.SelectedItems;

            m_lbEntitiesInCollection.Items.Clear();

            foreach (MEntity pmEntity in m_pmEntityCollection)
            {
                m_lbEntitiesInCollection.Items.Add(pmEntity);
            }

            foreach (object pmObject in pmSelectedItems)
            {
                int iIndex = m_lbEntitiesInCollection.Items.IndexOf(pmObject);
                if (iIndex > 0)
                {
                    m_lbEntitiesInCollection.SetSelected(iIndex, true);
                }
            }
            if (m_lbEntitiesInCollection.SelectedIndex == -1 &&
                m_lbEntitiesInCollection.Items.Count > 0)
            {
                m_lbEntitiesInCollection.SelectedIndex = 0;
            }
        }

        private void UpdateButtonStates()
        {
            m_btnAdd.Enabled = (m_lbEntitiesInScene.SelectedIndex > -1);
            m_btnRemove.Enabled = (m_lbEntitiesInCollection.SelectedIndex >
                -1);
        }

        private void Do_Add()
        {
            ArrayList pmSelectedItems = new ArrayList(
                m_lbEntitiesInScene.SelectedItems);

            foreach (object pmSelectedItem in pmSelectedItems)
            {
                m_pmEntityCollection.Add(pmSelectedItem);
                m_lbEntitiesInCollection.Items.Add(pmSelectedItem);
                m_lbEntitiesInScene.Items.Remove(pmSelectedItem);
            }

            // Select entities we just added in collection list box.
            m_lbEntitiesInCollection.SelectedItem = null;
            foreach (object pmSelectedItem in pmSelectedItems)
            {
                int iIndex = m_lbEntitiesInCollection.Items.IndexOf(
                    pmSelectedItem);
                if (iIndex > 0)
                {
                    m_lbEntitiesInCollection.SetSelected(iIndex, true);
                }
            }

            UpdateButtonStates();
        }

        private void Do_Remove()
        {
            ArrayList pmSelectedItems = new ArrayList(
                m_lbEntitiesInCollection.SelectedItems);

            foreach (object pmSelectedItem in pmSelectedItems)
            {
                m_pmEntityCollection.Remove(pmSelectedItem);
                m_lbEntitiesInCollection.Items.Remove(pmSelectedItem);
                m_lbEntitiesInScene.Items.Add(pmSelectedItem);
            }

            // Select entities we just removed in the scene list box.
            m_lbEntitiesInScene.SelectedItem = null;
            foreach (object pmSelectedItem in pmSelectedItems)
            {
                int iIndex = m_lbEntitiesInScene.Items.IndexOf(
                    pmSelectedItem);
                if (iIndex > 0)
                {
                    m_lbEntitiesInScene.SetSelected(iIndex, true);
                }
            }

            UpdateButtonStates();
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

		private void CopyToClipboardInList(ListBox lbEntities)
		{
			string strText = "";
			// copy list to clipboard
			for (int i = 0; i < lbEntities.Items.Count; i++)
			{
				if (lbEntities.GetSelected(i))
				{
					strText += lbEntities.Items[i].ToString();
					strText += "\r\n";
				}
			}
			if (strText != "")
			{
				Clipboard.Clear();
				Clipboard.SetText(strText);
			}
		}

		private void m_btnAdd_Click(object sender, System.EventArgs e)
        {
            Do_Add();
        }

        private void m_btnRemove_Click(object sender, System.EventArgs e)
        {
            Do_Remove();
        }

        private void m_lbEntitiesInScene_DoubleClick(object sender,
            System.EventArgs e)
        {
            Do_Add();
        }

        private void m_lbEntitiesInCollection_DoubleClick(object sender,
            System.EventArgs e)
        {
            Do_Remove();
        }

        private void m_lbEntitiesInScene_SelectedIndexChanged(object sender,
            System.EventArgs e)
        {
            UpdateButtonStates();
        }

        private void m_lbEntitiesInCollection_SelectedIndexChanged(
            object sender, System.EventArgs e)
        {
            UpdateButtonStates();
        }

		private void m_tbEntitiesInScene_TextChanged(object sender, EventArgs e)
		{
			if (m_bTextChangeCase == true)
			{
				m_bTextChangeCase = false;
				return;
			}
			string strText = m_tbEntitiesInScene.Text;
			SelectFromText(strText, m_lbEntitiesInScene);
			int iRet = strText.IndexOf("\r\n");
			if (iRet != -1)
			{
				string strSub = strText.Substring(0, iRet);
				m_bTextChangeCase = true;
				m_tbEntitiesInScene.Text = strSub;
			}
		}

		private void m_tbEntitiesInCollection_TextChanged(object sender, EventArgs e)
		{
			if (m_bTextChangeCase == true)
			{
				m_bTextChangeCase = false;
				return;
			}
			string strText = m_tbEntitiesInCollection.Text;
			SelectFromText(strText, m_lbEntitiesInCollection);
			int iRet = strText.IndexOf("\r\n");
			if (iRet != -1)
			{
				string strSub = strText.Substring(0, iRet);
				m_bTextChangeCase = true;
				m_tbEntitiesInCollection.Text = strSub;
			}
		}

		private void m_btnSelectAllEIS_Click(object sender, EventArgs e)
		{
			for (int i = 0; i < m_lbEntitiesInScene.Items.Count; i++)
			{
				m_lbEntitiesInScene.SetSelected(i, true);
			}
		}

		private void m_btnSelectRevertEIS_Click(object sender, EventArgs e)
		{
			for (int i = 0; i < m_lbEntitiesInScene.Items.Count; i++)
			{
				bool bSel = m_lbEntitiesInScene.GetSelected(i);
				m_lbEntitiesInScene.SetSelected(i, !bSel);
			}
		}

		private void m_btnSelectAllEIC_Click(object sender, EventArgs e)
		{
			for (int i = 0; i < m_lbEntitiesInCollection.Items.Count; i++)
			{
				m_lbEntitiesInCollection.SetSelected(i, true);
			}
		}

		private void m_btnSelectRevertEIC_Click(object sender, EventArgs e)
		{
			for (int i = 0; i < m_lbEntitiesInCollection.Items.Count; i++)
			{
				bool bSel = m_lbEntitiesInCollection.GetSelected(i);
				m_lbEntitiesInCollection.SetSelected(i, !bSel);
			}
		}

		private void m_btnCopyScene_Click(object sender, EventArgs e)
		{
			CopyToClipboardInList(m_lbEntitiesInScene);
		}

		private void m_btnCollection_Click(object sender, EventArgs e)
		{
			CopyToClipboardInList(m_lbEntitiesInCollection);
		}
	}
}

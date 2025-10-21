using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using NiManagedToolInterface;
using System.Reflection;

namespace AnimationToolUI
{
	/// <summary>
	/// Summary description for HelpForm.
	/// </summary>
	public class EventItemListForm : System.Windows.Forms.Form
	{
		private Label label1;
		private Label label2;
		
		private ColumnHeader chEventItem;
		private ColumnHeader chItemPath; 
		private ListView lvEventItemList;
		
		private TextBox txtEventListPath;
		private TextBox txtEventType;

		private Button btnSelect;
		private Button btnChangePath;
		private Button btnPreview;

		private OpenFileDialog ofdSelectEventList;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;
		private Button btnCancel;
		private string m_kSelectedItem = null;

		public EventItemListForm()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose(bool disposing)
		{
			if (disposing)
			{
				if (components != null)
				{
					components.Dispose();
				}
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
			this.label1 = new System.Windows.Forms.Label();
			this.lvEventItemList = new System.Windows.Forms.ListView();
			this.chEventItem = new System.Windows.Forms.ColumnHeader();
			this.chItemPath = new System.Windows.Forms.ColumnHeader();
			this.btnSelect = new System.Windows.Forms.Button();
			this.btnChangePath = new System.Windows.Forms.Button();
			this.txtEventListPath = new System.Windows.Forms.TextBox();
			this.txtEventType = new System.Windows.Forms.TextBox();
			this.label2 = new System.Windows.Forms.Label();
			this.btnPreview = new System.Windows.Forms.Button();
			this.ofdSelectEventList = new System.Windows.Forms.OpenFileDialog();
			this.btnCancel = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label1.Location = new System.Drawing.Point(12, 14);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(62, 13);
			this.label1.TabIndex = 0;
			this.label1.Text = "Event Type";
			// 
			// lvEventItemList
			// 
			this.lvEventItemList.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.lvEventItemList.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.chEventItem,
            this.chItemPath});
			this.lvEventItemList.FullRowSelect = true;
			this.lvEventItemList.Location = new System.Drawing.Point(12, 65);
			this.lvEventItemList.MultiSelect = false;
			this.lvEventItemList.Name = "lvEventItemList";
			this.lvEventItemList.Size = new System.Drawing.Size(377, 288);
			this.lvEventItemList.TabIndex = 1;
			this.lvEventItemList.UseCompatibleStateImageBehavior = false;
			this.lvEventItemList.View = System.Windows.Forms.View.Details;
			this.lvEventItemList.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.lvEventItemList_MouseDoubleClick);
			// 
			// chEventItem
			// 
			this.chEventItem.Text = "EventItem";
			this.chEventItem.Width = 70;
			// 
			// chItemPath
			// 
			this.chItemPath.Text = "ItemPath";
			this.chItemPath.Width = 263;
			// 
			// btnSelect
			// 
			this.btnSelect.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnSelect.Location = new System.Drawing.Point(12, 360);
			this.btnSelect.Name = "btnSelect";
			this.btnSelect.Size = new System.Drawing.Size(90, 23);
			this.btnSelect.TabIndex = 0;
			this.btnSelect.Text = "Select";
			this.btnSelect.UseVisualStyleBackColor = true;
			this.btnSelect.Click += new System.EventHandler(this.btnSelect_Click);
			// 
			// btnChangePath
			// 
			this.btnChangePath.Location = new System.Drawing.Point(310, 37);
			this.btnChangePath.Name = "btnChangePath";
			this.btnChangePath.Size = new System.Drawing.Size(79, 23);
			this.btnChangePath.TabIndex = 2;
			this.btnChangePath.Text = "Change Path";
			this.btnChangePath.UseVisualStyleBackColor = true;
			this.btnChangePath.Click += new System.EventHandler(this.btnChangePath_Click);
			// 
			// txtEventListPath
			// 
			this.txtEventListPath.Location = new System.Drawing.Point(97, 38);
			this.txtEventListPath.Name = "txtEventListPath";
			this.txtEventListPath.ReadOnly = true;
			this.txtEventListPath.Size = new System.Drawing.Size(207, 21);
			this.txtEventListPath.TabIndex = 3;
			// 
			// txtEventType
			// 
			this.txtEventType.Location = new System.Drawing.Point(97, 11);
			this.txtEventType.Name = "txtEventType";
			this.txtEventType.ReadOnly = true;
			this.txtEventType.Size = new System.Drawing.Size(98, 21);
			this.txtEventType.TabIndex = 4;
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label2.Location = new System.Drawing.Point(12, 41);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(79, 13);
			this.label2.TabIndex = 0;
			this.label2.Text = "Event List Path";
			// 
			// btnPreview
			// 
			this.btnPreview.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnPreview.Location = new System.Drawing.Point(108, 360);
			this.btnPreview.Name = "btnPreview";
			this.btnPreview.Size = new System.Drawing.Size(90, 23);
			this.btnPreview.TabIndex = 0;
			this.btnPreview.Text = "Preview";
			this.btnPreview.UseVisualStyleBackColor = true;
			this.btnPreview.Click += new System.EventHandler(this.btnPreview_Click);
			// 
			// ofdSelectEventList
			// 
			this.ofdSelectEventList.DefaultExt = "xml";
			this.ofdSelectEventList.Filter = "Xml file (*.xml)|*.xml";
			this.ofdSelectEventList.Title = "Open ..";
			// 
			// btnCancel
			// 
			this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.btnCancel.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnCancel.Location = new System.Drawing.Point(204, 360);
			this.btnCancel.Name = "btnCancel";
			this.btnCancel.Size = new System.Drawing.Size(90, 23);
			this.btnCancel.TabIndex = 0;
			this.btnCancel.Text = "Cancel";
			this.btnCancel.UseVisualStyleBackColor = true;
			this.btnCancel.Click += new System.EventHandler(this.btnPreview_Click);
			// 
			// EventItemListForm
			// 
			this.ClientSize = new System.Drawing.Size(401, 394);
			this.Controls.Add(this.txtEventType);
			this.Controls.Add(this.txtEventListPath);
			this.Controls.Add(this.btnChangePath);
			this.Controls.Add(this.btnCancel);
			this.Controls.Add(this.btnPreview);
			this.Controls.Add(this.btnSelect);
			this.Controls.Add(this.lvEventItemList);
			this.Controls.Add(this.label2);
			this.Controls.Add(this.label1);
			this.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.Name = "EventItemListForm";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "Select Event Item List..";
			this.Load += new System.EventHandler(this.EventItemListForm_Load);
			this.ResumeLayout(false);
			this.PerformLayout();

		}
		#endregion

		private void EventItemListForm_Load(object sender, EventArgs e)
		{

		}

		private void btnChangePath_Click(object sender, EventArgs e)
		{
			if (ofdSelectEventList.ShowDialog() == DialogResult.OK)
			{
				ArrayList akEventList = null;
				if (txtEventType.Text == "Sound")
				{
					MFramework.Instance.AudioMan.DeleteContents();
					if (!MFramework.Instance.AudioMan.AddEventItems(ofdSelectEventList.FileName, "SOUND"))
					{
						return;
					}
					txtEventListPath.Text = ofdSelectEventList.FileName;
					akEventList = MFramework.Instance.AudioMan.GetEventPathList();
				}
				else if (txtEventType.Text == "Effect")
				{
					MFramework.Instance.EffectMan.DeleteContents();
					if (!MFramework.Instance.EffectMan.AddEventItems(ofdSelectEventList.FileName, "EFFECT"))
					{
						return;
					}
					txtEventListPath.Text = ofdSelectEventList.FileName;
					akEventList = MFramework.Instance.EffectMan.GetEventPathList();
				}

				UpdateEventList(ref akEventList);
			}
		}

		private void btnSelect_Click(object sender, EventArgs e)
		{
			if (lvEventItemList.SelectedItems.Count == 0)
			{
				return;
			}

			m_kSelectedItem = (string)lvEventItemList.SelectedItems[0].Text;
			this.DialogResult = DialogResult.OK;
		}

		public void UpdateEventList(ref ArrayList akEventList)
		{
			lvEventItemList.Items.Clear();
			foreach (DictionaryEntry kEventItem in akEventList)
			{
				ListViewItem kItem = new ListViewItem(
					new string[] { 
						kEventItem.Key.ToString(), 
						kEventItem.Value.ToString() 
					},
					-1);
				lvEventItemList.Items.Add(kItem);
			}
		}
		public void SetAdditionalInfo(string kPath, string kType)
		{
			txtEventListPath.Text = kPath;
			txtEventType.Text = kType;
		}

		public string GetSelectedItem()
		{
			return m_kSelectedItem;
		}

		private void lvEventItemList_MouseDoubleClick(object sender, MouseEventArgs e)
		{
			btnSelect_Click(sender, e);
		}

		private void btnPreview_Click(object sender, EventArgs e)
		{
			
		}
	}
}

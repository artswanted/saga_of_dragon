using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using NiManagedToolInterface;

namespace AnimationToolUI
{
    class ItemForm : System.Windows.Forms.Form
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
		private System.Windows.Forms.Label label6;

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox2;
        
        private System.Windows.Forms.ListView lvSrcTexture;

        private System.Windows.Forms.TextBox txtItemName;
        private System.Windows.Forms.TextBox txtKfmPath;
        private System.Windows.Forms.TextBox txtNifPath;
        private System.Windows.Forms.TextBox txtDest;
		private System.Windows.Forms.TextBox txtBasePath;
        
        private System.Windows.Forms.Button btnNifBrowse;
        private System.Windows.Forms.Button btnKfmBrowse;
        private System.Windows.Forms.Button btnGetSrcTexture;
        private System.Windows.Forms.Button btnDestBrowse;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.Button btnSave;
        private System.Windows.Forms.Button btnGetName;
		private System.Windows.Forms.Button btnBrowseBasePath;
		private System.Windows.Forms.Button btnReset;
		private System.Windows.Forms.Button btnSaveAs;
		private System.Windows.Forms.Button btnMakeNewItem;
		private System.Windows.Forms.Button btnSetDestTexture;
        
        private System.Windows.Forms.OpenFileDialog ofdSelectFile;
		private System.Windows.Forms.FolderBrowserDialog ofdSelectFolder;
		
		private ColumnHeader chSrcTexture;
		private ColumnSort m_kColumnSorter = new ColumnSort();

		private MItem m_kItem = null;

		public enum EditMode { NEW, MODIFY };
		private EditMode eEditMode = EditMode.NEW;
		private SaveFileDialog sfdSelectFile;

		private bool m_bMarkAsChange = false;
		private string m_kSaveXmlPath = null;
		private string m_kSaveXmlFullPath = null;

        public ItemForm()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			// Set Column Sorter.
			lvSrcTexture.ListViewItemSorter = m_kColumnSorter;
		}

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
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
			this.label1 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.label3 = new System.Windows.Forms.Label();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.btnNifBrowse = new System.Windows.Forms.Button();
			this.btnGetName = new System.Windows.Forms.Button();
			this.btnBrowseBasePath = new System.Windows.Forms.Button();
			this.btnKfmBrowse = new System.Windows.Forms.Button();
			this.txtNifPath = new System.Windows.Forms.TextBox();
			this.txtKfmPath = new System.Windows.Forms.TextBox();
			this.txtBasePath = new System.Windows.Forms.TextBox();
			this.txtItemName = new System.Windows.Forms.TextBox();
			this.label6 = new System.Windows.Forms.Label();
			this.groupBox2 = new System.Windows.Forms.GroupBox();
			this.btnSetDestTexture = new System.Windows.Forms.Button();
			this.btnGetSrcTexture = new System.Windows.Forms.Button();
			this.btnDestBrowse = new System.Windows.Forms.Button();
			this.txtDest = new System.Windows.Forms.TextBox();
			this.lvSrcTexture = new System.Windows.Forms.ListView();
			this.chSrcTexture = new System.Windows.Forms.ColumnHeader();
			this.label5 = new System.Windows.Forms.Label();
			this.label4 = new System.Windows.Forms.Label();
			this.btnCancel = new System.Windows.Forms.Button();
			this.btnSave = new System.Windows.Forms.Button();
			this.ofdSelectFile = new System.Windows.Forms.OpenFileDialog();
			this.btnReset = new System.Windows.Forms.Button();
			this.btnSaveAs = new System.Windows.Forms.Button();
			this.btnMakeNewItem = new System.Windows.Forms.Button();
			this.ofdSelectFolder = new System.Windows.Forms.FolderBrowserDialog();
			this.sfdSelectFile = new System.Windows.Forms.SaveFileDialog();
			this.groupBox1.SuspendLayout();
			this.groupBox2.SuspendLayout();
			this.SuspendLayout();
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label1.Location = new System.Drawing.Point(19, 55);
			this.label1.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(59, 13);
			this.label1.TabIndex = 0;
			this.label1.Text = "Item Name";
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label2.Location = new System.Drawing.Point(19, 85);
			this.label2.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(52, 13);
			this.label2.TabIndex = 1;
			this.label2.Text = "KFM Path";
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label3.Location = new System.Drawing.Point(19, 114);
			this.label3.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(49, 13);
			this.label3.TabIndex = 1;
			this.label3.Text = "NIF Path";
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.btnNifBrowse);
			this.groupBox1.Controls.Add(this.btnGetName);
			this.groupBox1.Controls.Add(this.btnBrowseBasePath);
			this.groupBox1.Controls.Add(this.btnKfmBrowse);
			this.groupBox1.Controls.Add(this.txtNifPath);
			this.groupBox1.Controls.Add(this.txtKfmPath);
			this.groupBox1.Controls.Add(this.txtBasePath);
			this.groupBox1.Controls.Add(this.txtItemName);
			this.groupBox1.Controls.Add(this.label2);
			this.groupBox1.Controls.Add(this.label3);
			this.groupBox1.Controls.Add(this.label6);
			this.groupBox1.Controls.Add(this.label1);
			this.groupBox1.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.groupBox1.Location = new System.Drawing.Point(13, 13);
			this.groupBox1.Margin = new System.Windows.Forms.Padding(5, 3, 5, 3);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Padding = new System.Windows.Forms.Padding(5, 3, 5, 3);
			this.groupBox1.Size = new System.Drawing.Size(901, 150);
			this.groupBox1.TabIndex = 0;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Base Information";
			// 
			// btnNifBrowse
			// 
			this.btnNifBrowse.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnNifBrowse.Location = new System.Drawing.Point(799, 109);
			this.btnNifBrowse.Margin = new System.Windows.Forms.Padding(5, 3, 5, 3);
			this.btnNifBrowse.Name = "btnNifBrowse";
			this.btnNifBrowse.Size = new System.Drawing.Size(31, 24);
			this.btnNifBrowse.TabIndex = 9;
			this.btnNifBrowse.Text = "...";
			this.btnNifBrowse.UseVisualStyleBackColor = true;
			this.btnNifBrowse.Click += new System.EventHandler(this.btnNifBrowse_Click);
			// 
			// btnGetName
			// 
			this.btnGetName.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnGetName.Location = new System.Drawing.Point(373, 52);
			this.btnGetName.Margin = new System.Windows.Forms.Padding(5, 3, 5, 3);
			this.btnGetName.Name = "btnGetName";
			this.btnGetName.Size = new System.Drawing.Size(134, 25);
			this.btnGetName.TabIndex = 5;
			this.btnGetName.Text = "Get From XML File&Name!";
			this.btnGetName.UseVisualStyleBackColor = true;
			this.btnGetName.Click += new System.EventHandler(this.btnGetName_Click);
			// 
			// btnBrowseBasePath
			// 
			this.btnBrowseBasePath.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnBrowseBasePath.Location = new System.Drawing.Point(373, 21);
			this.btnBrowseBasePath.Margin = new System.Windows.Forms.Padding(5, 3, 5, 3);
			this.btnBrowseBasePath.Name = "btnBrowseBasePath";
			this.btnBrowseBasePath.Size = new System.Drawing.Size(31, 24);
			this.btnBrowseBasePath.TabIndex = 3;
			this.btnBrowseBasePath.Text = "...";
			this.btnBrowseBasePath.UseVisualStyleBackColor = true;
			this.btnBrowseBasePath.Click += new System.EventHandler(this.btnBrowseBasePath_Click);
			// 
			// btnKfmBrowse
			// 
			this.btnKfmBrowse.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnKfmBrowse.Location = new System.Drawing.Point(799, 80);
			this.btnKfmBrowse.Margin = new System.Windows.Forms.Padding(5, 3, 5, 3);
			this.btnKfmBrowse.Name = "btnKfmBrowse";
			this.btnKfmBrowse.Size = new System.Drawing.Size(31, 24);
			this.btnKfmBrowse.TabIndex = 7;
			this.btnKfmBrowse.Text = "...";
			this.btnKfmBrowse.UseVisualStyleBackColor = true;
			this.btnKfmBrowse.Click += new System.EventHandler(this.btnKfmBrowse_Click);
			// 
			// txtNifPath
			// 
			this.txtNifPath.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.txtNifPath.Location = new System.Drawing.Point(103, 111);
			this.txtNifPath.Margin = new System.Windows.Forms.Padding(5, 3, 5, 3);
			this.txtNifPath.Name = "txtNifPath";
			this.txtNifPath.Size = new System.Drawing.Size(693, 21);
			this.txtNifPath.TabIndex = 8;
			this.txtNifPath.Leave += new System.EventHandler(this.txtNifPath_Leave);
			// 
			// txtKfmPath
			// 
			this.txtKfmPath.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.txtKfmPath.Location = new System.Drawing.Point(103, 81);
			this.txtKfmPath.Margin = new System.Windows.Forms.Padding(5, 3, 5, 3);
			this.txtKfmPath.Name = "txtKfmPath";
			this.txtKfmPath.Size = new System.Drawing.Size(693, 21);
			this.txtKfmPath.TabIndex = 6;
			// 
			// txtBasePath
			// 
			this.txtBasePath.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.txtBasePath.Location = new System.Drawing.Point(103, 22);
			this.txtBasePath.Margin = new System.Windows.Forms.Padding(5, 3, 5, 3);
			this.txtBasePath.Name = "txtBasePath";
			this.txtBasePath.ReadOnly = true;
			this.txtBasePath.Size = new System.Drawing.Size(263, 21);
			this.txtBasePath.TabIndex = 2;
			// 
			// txtItemName
			// 
			this.txtItemName.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.txtItemName.Location = new System.Drawing.Point(103, 52);
			this.txtItemName.Margin = new System.Windows.Forms.Padding(5, 3, 5, 3);
			this.txtItemName.Name = "txtItemName";
			this.txtItemName.Size = new System.Drawing.Size(263, 21);
			this.txtItemName.TabIndex = 4;
			this.txtItemName.Leave += new System.EventHandler(this.txtItemName_Leave);
			// 
			// label6
			// 
			this.label6.AutoSize = true;
			this.label6.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label6.Location = new System.Drawing.Point(19, 25);
			this.label6.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
			this.label6.Name = "label6";
			this.label6.Size = new System.Drawing.Size(55, 13);
			this.label6.TabIndex = 0;
			this.label6.Text = "Base Path";
			// 
			// groupBox2
			// 
			this.groupBox2.Controls.Add(this.btnSetDestTexture);
			this.groupBox2.Controls.Add(this.btnGetSrcTexture);
			this.groupBox2.Controls.Add(this.btnDestBrowse);
			this.groupBox2.Controls.Add(this.txtDest);
			this.groupBox2.Controls.Add(this.lvSrcTexture);
			this.groupBox2.Controls.Add(this.label5);
			this.groupBox2.Controls.Add(this.label4);
			this.groupBox2.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.groupBox2.Location = new System.Drawing.Point(13, 169);
			this.groupBox2.Margin = new System.Windows.Forms.Padding(5, 3, 5, 3);
			this.groupBox2.Name = "groupBox2";
			this.groupBox2.Padding = new System.Windows.Forms.Padding(5, 3, 5, 3);
			this.groupBox2.Size = new System.Drawing.Size(901, 184);
			this.groupBox2.TabIndex = 1;
			this.groupBox2.TabStop = false;
			this.groupBox2.Text = "Texturing Property";
			// 
			// btnSetDestTexture
			// 
			this.btnSetDestTexture.Enabled = false;
			this.btnSetDestTexture.Location = new System.Drawing.Point(833, 150);
			this.btnSetDestTexture.Margin = new System.Windows.Forms.Padding(5, 3, 5, 3);
			this.btnSetDestTexture.Name = "btnSetDestTexture";
			this.btnSetDestTexture.Size = new System.Drawing.Size(59, 24);
			this.btnSetDestTexture.TabIndex = 6;
			this.btnSetDestTexture.Text = "Set";
			this.btnSetDestTexture.UseVisualStyleBackColor = true;
			this.btnSetDestTexture.Click += new System.EventHandler(this.btnSetDestTexture_Click);
			// 
			// btnGetSrcTexture
			// 
			this.btnGetSrcTexture.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnGetSrcTexture.Location = new System.Drawing.Point(504, 22);
			this.btnGetSrcTexture.Margin = new System.Windows.Forms.Padding(5, 3, 5, 3);
			this.btnGetSrcTexture.Name = "btnGetSrcTexture";
			this.btnGetSrcTexture.Size = new System.Drawing.Size(101, 122);
			this.btnGetSrcTexture.TabIndex = 3;
			this.btnGetSrcTexture.Text = "&Refresh";
			this.btnGetSrcTexture.UseVisualStyleBackColor = true;
			this.btnGetSrcTexture.Click += new System.EventHandler(this.btnGetSrcTexture_Click);
			// 
			// btnDestBrowse
			// 
			this.btnDestBrowse.Enabled = false;
			this.btnDestBrowse.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnDestBrowse.Location = new System.Drawing.Point(799, 150);
			this.btnDestBrowse.Margin = new System.Windows.Forms.Padding(5, 3, 5, 3);
			this.btnDestBrowse.Name = "btnDestBrowse";
			this.btnDestBrowse.Size = new System.Drawing.Size(31, 24);
			this.btnDestBrowse.TabIndex = 5;
			this.btnDestBrowse.Text = "...";
			this.btnDestBrowse.UseVisualStyleBackColor = true;
			this.btnDestBrowse.Click += new System.EventHandler(this.btnDestBrowse_Click);
			// 
			// txtDest
			// 
			this.txtDest.Enabled = false;
			this.txtDest.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.txtDest.Location = new System.Drawing.Point(103, 151);
			this.txtDest.Margin = new System.Windows.Forms.Padding(5, 3, 5, 3);
			this.txtDest.Multiline = true;
			this.txtDest.Name = "txtDest";
			this.txtDest.ReadOnly = true;
			this.txtDest.Size = new System.Drawing.Size(693, 22);
			this.txtDest.TabIndex = 4;
			// 
			// lvSrcTexture
			// 
			this.lvSrcTexture.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.chSrcTexture});
			this.lvSrcTexture.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lvSrcTexture.FullRowSelect = true;
			this.lvSrcTexture.GridLines = true;
			this.lvSrcTexture.Location = new System.Drawing.Point(103, 22);
			this.lvSrcTexture.Margin = new System.Windows.Forms.Padding(5, 3, 5, 3);
			this.lvSrcTexture.MultiSelect = false;
			this.lvSrcTexture.Name = "lvSrcTexture";
			this.lvSrcTexture.Size = new System.Drawing.Size(393, 122);
			this.lvSrcTexture.TabIndex = 2;
			this.lvSrcTexture.UseCompatibleStateImageBehavior = false;
			this.lvSrcTexture.View = System.Windows.Forms.View.Details;
			this.lvSrcTexture.SelectedIndexChanged += new System.EventHandler(this.lvSrcTexture_SelectedIndexChanged);
			this.lvSrcTexture.ColumnClick += new System.Windows.Forms.ColumnClickEventHandler(this.lvSrcTexture_ColumnClick);
			// 
			// chSrcTexture
			// 
			this.chSrcTexture.Text = "Source Texture Path";
			this.chSrcTexture.Width = 300;
			// 
			// label5
			// 
			this.label5.AutoSize = true;
			this.label5.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label5.Location = new System.Drawing.Point(37, 154);
			this.label5.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(29, 13);
			this.label5.TabIndex = 1;
			this.label5.Text = "Dest";
			// 
			// label4
			// 
			this.label4.AutoSize = true;
			this.label4.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label4.Location = new System.Drawing.Point(37, 78);
			this.label4.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(22, 13);
			this.label4.TabIndex = 1;
			this.label4.Text = "Src";
			// 
			// btnCancel
			// 
			this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.btnCancel.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnCancel.Location = new System.Drawing.Point(503, 360);
			this.btnCancel.Margin = new System.Windows.Forms.Padding(5, 3, 5, 3);
			this.btnCancel.Name = "btnCancel";
			this.btnCancel.Size = new System.Drawing.Size(117, 25);
			this.btnCancel.TabIndex = 6;
			this.btnCancel.Text = "&Cancel";
			this.btnCancel.UseVisualStyleBackColor = true;
			// 
			// btnSave
			// 
			this.btnSave.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnSave.Location = new System.Drawing.Point(136, 360);
			this.btnSave.Margin = new System.Windows.Forms.Padding(5, 3, 5, 3);
			this.btnSave.Name = "btnSave";
			this.btnSave.Size = new System.Drawing.Size(117, 25);
			this.btnSave.TabIndex = 3;
			this.btnSave.Text = "&Save";
			this.btnSave.UseVisualStyleBackColor = true;
			this.btnSave.Click += new System.EventHandler(this.btnSave_Click);
			// 
			// ofdSelectFile
			// 
			this.ofdSelectFile.RestoreDirectory = true;
			this.ofdSelectFile.Title = "Select File..";
			// 
			// btnReset
			// 
			this.btnReset.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnReset.Location = new System.Drawing.Point(379, 360);
			this.btnReset.Margin = new System.Windows.Forms.Padding(5, 3, 5, 3);
			this.btnReset.Name = "btnReset";
			this.btnReset.Size = new System.Drawing.Size(117, 25);
			this.btnReset.TabIndex = 5;
			this.btnReset.Text = "&Reset";
			this.btnReset.UseVisualStyleBackColor = true;
			this.btnReset.Click += new System.EventHandler(this.btnReset_Click);
			// 
			// btnSaveAs
			// 
			this.btnSaveAs.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnSaveAs.Location = new System.Drawing.Point(259, 360);
			this.btnSaveAs.Margin = new System.Windows.Forms.Padding(5, 3, 5, 3);
			this.btnSaveAs.Name = "btnSaveAs";
			this.btnSaveAs.Size = new System.Drawing.Size(117, 25);
			this.btnSaveAs.TabIndex = 4;
			this.btnSaveAs.Text = "Save &As..";
			this.btnSaveAs.UseVisualStyleBackColor = true;
			this.btnSaveAs.Click += new System.EventHandler(this.btnSaveAs_Click);
			// 
			// btnMakeNewItem
			// 
			this.btnMakeNewItem.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btnMakeNewItem.Location = new System.Drawing.Point(13, 360);
			this.btnMakeNewItem.Margin = new System.Windows.Forms.Padding(5, 3, 5, 3);
			this.btnMakeNewItem.Name = "btnMakeNewItem";
			this.btnMakeNewItem.Size = new System.Drawing.Size(117, 25);
			this.btnMakeNewItem.TabIndex = 2;
			this.btnMakeNewItem.Text = "Make &New Item";
			this.btnMakeNewItem.UseVisualStyleBackColor = true;
			this.btnMakeNewItem.Click += new System.EventHandler(this.btnMakeNewItem_Click);
			// 
			// sfdSelectFile
			// 
			this.sfdSelectFile.RestoreDirectory = true;
			this.sfdSelectFile.Title = "Save Xml File..";
			// 
			// ItemForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.CancelButton = this.btnCancel;
			this.ClientSize = new System.Drawing.Size(935, 397);
			this.Controls.Add(this.btnSaveAs);
			this.Controls.Add(this.btnReset);
			this.Controls.Add(this.btnCancel);
			this.Controls.Add(this.btnMakeNewItem);
			this.Controls.Add(this.btnSave);
			this.Controls.Add(this.groupBox2);
			this.Controls.Add(this.groupBox1);
			this.Font = new System.Drawing.Font("Gulim", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.Margin = new System.Windows.Forms.Padding(5, 3, 5, 3);
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "ItemForm";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "ItemForm";
			this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.ItemForm_KeyDown);
			this.Load += new System.EventHandler(this.ItemForm_Load);
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			this.groupBox2.ResumeLayout(false);
			this.groupBox2.PerformLayout();
			this.ResumeLayout(false);

        }

        #endregion

		private void ItemForm_Load(object sender, EventArgs e)
		{
			string kBasePath = "";
			if (AppUtils.RestoreRegKey("BasePath", ref kBasePath))
			{
				txtBasePath.Text = kBasePath;
			}
			InitializeItemForm();
		}

		private void InitializeItemForm()
		{
			if (eEditMode == EditMode.NEW)
			{
				// Something to do..								
			}
			else if (eEditMode == EditMode.MODIFY && m_kItem != null)
			{
				this.Text = m_kSaveXmlFullPath = m_kItem.Path;
				txtItemName.Text = m_kItem.Name;
				txtNifPath.Text = m_kItem.NIFPath;
				txtKfmPath.Text = m_kItem.KFMPath;

				// Texture 설정
				ArrayList kSrcTextureList = m_kItem.GetSrcTexture();
				lvSrcTexture.Items.Clear();

				foreach (String kSrcTexturePath in kSrcTextureList)
				{
					ListViewItem item = new ListViewItem(new string[] { kSrcTexturePath }, -1);
					item.Tag = m_kItem.GetDestTexture(kSrcTexturePath);
					lvSrcTexture.Items.Add(item);
				}

				lvSrcTexture.Sort();

				ValidateItem(false);
			}
		}

		public void SetMode(EditMode kMode)
		{
			eEditMode = kMode;
		}

		public void SetTargetItem(MItem kItem)
		{
			m_kItem = kItem;
		}

		public void SetXmlPath(string kPath)
		{
			m_kSaveXmlPath = kPath;
		}

		public string GetXmlFullPath()
		{
			return m_kSaveXmlFullPath;
		}

		public string GetXmlPath()
		{
			return m_kSaveXmlPath;
		}

		public void RefreshSrcTexture()
		{
			if (m_kItem == null)
			{
				return;
			}

			lvSrcTexture.Items.Clear();

			// 갱신된 정보를 MItem에 올린 다음, 변경한다.
			ArrayList kSrcTextureList = m_kItem.GetSrcTexture();

			if (kSrcTextureList == null)
			{
				return;
			}

			foreach (string kSrcTexturePath in kSrcTextureList)
			{
				ListViewItem item = new ListViewItem(new string[] { kSrcTexturePath }, -1);
				item.Tag = m_kItem.GetDestTexture(kSrcTexturePath);
				lvSrcTexture.Items.Add(item);
			}
			lvSrcTexture.Sort();
		}
		
		private void lvSrcTexture_ColumnClick(object sender, ColumnClickEventArgs e)
		{
			ColumnSort.ListView_ColumnClick(sender, e, m_kColumnSorter);
		}

		private void lvSrcTexture_SelectedIndexChanged(object sender, EventArgs e)
		{
			ListViewItem kItem = ListViewUtil.GetFirstSelectedItem(ref lvSrcTexture);
			if (kItem != null)
			{
				txtDest.Text = (string)kItem.Tag;
				txtDest.ReadOnly = false;
				txtDest.Enabled = true;
				btnDestBrowse.Enabled = true;
				btnSetDestTexture.Enabled = true;
			}
			else
			{
				txtDest.Text = "";
				txtDest.ReadOnly = true;
				txtDest.Enabled = false;
				btnDestBrowse.Enabled = false;
				btnSetDestTexture.Enabled = false;
			}
		}

		private void btnGetSrcTexture_Click(object sender, EventArgs e)
		{
			RefreshSrcTexture();
		}

		private void btnBrowseBasePath_Click(object sender, EventArgs e)
		{
			ofdSelectFolder.ShowNewFolderButton = true;
			ofdSelectFolder.SelectedPath = txtBasePath.Text;
			DialogResult kResult = ofdSelectFolder.ShowDialog();
			if (kResult == DialogResult.OK)
			{
				txtBasePath.Text = ofdSelectFolder.SelectedPath;
				AppUtils.StoreRegKey("BasePath", txtBasePath.Text);
			}
		}

		private void btnKfmBrowse_Click(object sender, EventArgs e)
		{
			ofdSelectFile.FileName = "";
			ofdSelectFile.DefaultExt = "kfm";
			ofdSelectFile.Filter = "KFM file (*.kfm)|*.kfm";
			ofdSelectFile.Title = "Find KFM File";
			ofdSelectFile.Multiselect = false; ;

			string kKfmDefaultPath = "";
			if (AppUtils.RestoreRegKey("KfmBrowsePath", ref kKfmDefaultPath))
			{
				ofdSelectFile.InitialDirectory = kKfmDefaultPath;
			}

			DialogResult kResult = ofdSelectFile.ShowDialog();
			if (kResult == DialogResult.OK)
			{
				txtKfmPath.Text = AppUtils.CreateRelativePath(ofdSelectFile.FileName, txtBasePath.Text, false);
				AppUtils.StoreRegKey("KfmBrowsePath", AppUtils.ExtractDirFromPath(ofdSelectFile.FileName));
			}
		}

		private void ChangeNifPath(string kNewNifPath)
		{
			string kNewNif = AppUtils.CreateRelativePath(kNewNifPath, txtBasePath.Text, false).Trim();

			if (m_kItem != null)
			{
				// 기존의 아이템이 있을 경우 
				if (m_kItem.NIFPath != kNewNif)
				{
					if (m_kItem.ChangeMesh(kNewNif))
					{
						txtNifPath.Text = kNewNif;
						txtDest.Text = "";
					}
					else
					{
						MessageBox.Show("NifPath : 지정된 경로의 메쉬를 로드할 수 없습니다.");
						txtNifPath.Text = m_kItem.NIFPath;
						return;
					}
					
					RefreshSrcTexture();
				}
			}
			else
			{
				// 새 아이템을 생성할 경우
				if(txtItemName.Text.Length == 0)
				{
					txtItemName.Text = "None";
				}

				m_kItem = new MItem(txtItemName.Text, kNewNif);
				if (m_kItem.NIFPath != null)
				{
					txtNifPath.Text = kNewNif;
					RefreshSrcTexture();
				}
			}
		}

		private void txtNifPath_Leave(object sender, EventArgs e)
		{
			if (txtNifPath.Text.Trim().Length != 0)
			{
				ChangeNifPath(txtNifPath.Text);
			}
		}
		
		private void btnNifBrowse_Click(object sender, EventArgs e)
		{
			ofdSelectFile.FileName = "";
			ofdSelectFile.DefaultExt = "nif";
			ofdSelectFile.Filter = "NIF file (*.nif)|*.nif";
			ofdSelectFile.Title = "Find NIF File";
			ofdSelectFile.Multiselect = false;
			
			string kNifDefaultPath = "";
			if (AppUtils.RestoreRegKey("NifBrowsePath", ref kNifDefaultPath))
			{
				ofdSelectFile.InitialDirectory = kNifDefaultPath;
			}

			DialogResult kResult = ofdSelectFile.ShowDialog();
			if (kResult == DialogResult.OK)
			{
				ChangeNifPath(ofdSelectFile.FileName);
				AppUtils.StoreRegKey("NifBrowsePath", AppUtils.ExtractDirFromPath(ofdSelectFile.FileName));
			}
		}

		private void btnDestBrowse_Click(object sender, EventArgs e)
		{
			ofdSelectFile.FileName = "";
			ofdSelectFile.DefaultExt = "dds";
			ofdSelectFile.Filter = "DDS file (*.dds)|*.dds|TGA file (*.tga)|*.tga";
			ofdSelectFile.Title = "Find DDS File";
			ofdSelectFile.Multiselect = false;
			
			string kDestTextureDefaultPath = "";
			if (AppUtils.RestoreRegKey("DestTextureBrowsePath", ref kDestTextureDefaultPath))
			{
				ofdSelectFile.InitialDirectory = kDestTextureDefaultPath;
			}

			DialogResult kResult = ofdSelectFile.ShowDialog();
			if (kResult == DialogResult.OK)
			{
				txtDest.Text = AppUtils.CreateRelativePath(ofdSelectFile.FileName, txtBasePath.Text, false);
				AppUtils.StoreRegKey("DestTextureBrowsePath", AppUtils.ExtractDirFromPath(ofdSelectFile.FileName));
				// TODO : 갱신된 데이터를 버퍼에 저장~

			}
		}

		private void btnSaveAs_Click(object sender, EventArgs e)
		{
			sfdSelectFile.DefaultExt = "xml";
			sfdSelectFile.Filter = "Xml file (*.xml)|*.xml";
			sfdSelectFile.Title = "Save As..";

			string kSaveAsDefaultPath = "";
			if (AppUtils.RestoreRegKey("SaveAsPath", ref kSaveAsDefaultPath))
			{
				sfdSelectFile.InitialDirectory = kSaveAsDefaultPath;
			}

			DialogResult kResult = sfdSelectFile.ShowDialog();
			if (kResult == DialogResult.OK)
			{
				if (ValidateItem(true))
				{
					m_kItem.WriteToXml(sfdSelectFile.FileName, true);
					AppUtils.StoreRegKey("SaveAsPath", AppUtils.ExtractDirFromPath(sfdSelectFile.FileName));
					this.DialogResult = DialogResult.OK;
				}
			}
		}

		private void btnSave_Click(object sender, EventArgs e)
		{
			if (m_kSaveXmlFullPath == null)
			{
				sfdSelectFile.DefaultExt = "xml";
				sfdSelectFile.Filter = "Xml file (*.xml)|*.xml";
				sfdSelectFile.Title = "Save ..";
				if (m_kSaveXmlPath != null)
				{
					sfdSelectFile.InitialDirectory = m_kSaveXmlPath;
				}

				DialogResult kResult = sfdSelectFile.ShowDialog();
				if (kResult == DialogResult.OK)
				{
					m_kSaveXmlPath = AppUtils.ExtractDirFromPath(sfdSelectFile.FileName);
					// 마지막에 BackSlahs를 빼준다.
					m_kSaveXmlPath = m_kSaveXmlPath.Remove(m_kSaveXmlPath.Length - 1);
					m_kSaveXmlFullPath = sfdSelectFile.FileName;
				}
				else
				{
					return;
				}
			}

			if (ValidateItem(true))
			{
				m_kItem.WriteToXml(m_kSaveXmlFullPath, true);
				this.DialogResult = System.Windows.Forms.DialogResult.OK;
			}
		}

		private void btnSetDestTexture_Click(object sender, EventArgs e)
		{
			string kDest = txtDest.Text.Trim();
			ListViewItem kSrcTexture = ListViewUtil.GetFirstSelectedItem(ref lvSrcTexture);
			if (kSrcTexture == null || m_kItem == null)
			{
				return;
			}

			if (!m_kItem.SetDestTexture(kSrcTexture.Text, kDest))
			{
				MessageBox.Show("대상 텍스쳐가 유효한 경로가 아닙니다. \n\r - " + kDest);
				kSrcTexture.BackColor = Color.Orange;
				return;
			}

			kSrcTexture.BackColor = Color.White;
			kSrcTexture.Tag = kDest;
		}

		private bool ValidateItem(bool bShowMessageBox)
		{
			if (m_kItem == null)
			{
				return false;
			}

			ArrayList kSrcTextureList = m_kItem.GetSrcTexture();
			if (kSrcTextureList == null)
			{
				// 변경할 텍스쳐가 없다!?
				return false;
			}

			string kTextureErrorMessage = null;
			bool bTextureValid = true;
			foreach (string kSrcTexturePath in kSrcTextureList)
			{
				string kErrMsg = null;
				if (!ValidateTexture(kSrcTexturePath, ref kErrMsg))
				{
					bTextureValid = false;
					kTextureErrorMessage += kErrMsg;
				}
			}

			if (!bTextureValid && bShowMessageBox)
			{
				kTextureErrorMessage = kTextureErrorMessage.Insert(0, "※ 변경할 대상 텍스쳐가 존재하지 않습니다\n\r");
				MessageBox.Show(kTextureErrorMessage, "Texture Information Invalid");
				return false;
			}
			else if(bTextureValid)
			{
				foreach (ListViewItem kItem in lvSrcTexture.Items)
				{
					kItem.BackColor = Color.White;
				}
			}

			return true;
		}

		private bool ValidateTexture(string kSrcTexturePath, ref string kErrMsg)
		{
			ListViewItem kSrcItem = lvSrcTexture.FindItemWithText(kSrcTexturePath);
			string kDestTexturePath = (string) kSrcItem.Tag;
			bool bValidTexture = true;

			if (kSrcItem != null && m_kItem != null && kDestTexturePath != null)
			{
				if (!m_kItem.IsExistsTexture(kDestTexturePath.Trim()) ||
					kDestTexturePath.Length == 0)
				{
					bValidTexture = false;
					
				}
			}
			else if (kDestTexturePath == null)
			{
				bValidTexture = false;
			}

			if (bValidTexture)
			{
				kSrcItem.BackColor = Color.White;
			}
			else
			{
				kSrcItem.BackColor = Color.Orange;
				kErrMsg = "  ­ " + kDestTexturePath + "\n";
			}

			return bValidTexture;
		}

		private void btnMakeNewItem_Click(object sender, EventArgs e)
		{
			this.Text = "Make New Item";
			m_kItem = null;
			txtItemName.Text = "";
			txtNifPath.Text = "";
			txtKfmPath.Text = "";
			m_kSaveXmlPath = "";
			m_kSaveXmlFullPath = "";

			txtDest.Text = "";
			txtDest.ReadOnly = true;
			txtDest.Enabled = false;
			btnDestBrowse.Enabled = false;
			btnSetDestTexture.Enabled = false;

			lvSrcTexture.Items.Clear();

			eEditMode = EditMode.NEW;
			InitializeItemForm();
		}

		private void btnReset_Click(object sender, EventArgs e)
		{
			
		}

		private void ItemForm_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Escape)
			{
				if (m_bMarkAsChange)
				{
					DialogResult kRet = MessageBox.Show("변경된 내용이 있습니다. 저장하시겠습니까?", "Warning!", MessageBoxButtons.YesNoCancel);
					if (kRet == DialogResult.Yes)
					{
						// Save!

						this.DialogResult = DialogResult.OK;
					}
					else
					{
						this.DialogResult = DialogResult.Cancel;
					}
				}
			}
		}

		private void btnGetName_Click(object sender, EventArgs e)
		{
			if (m_kSaveXmlFullPath != null)
			{
				string kItemName = AppUtils.ExtractFileFromPath(m_kSaveXmlFullPath);
				string kDelim = ".";
				string[] kSplitItemName = kItemName.Split(kDelim.ToCharArray());
				txtItemName.Text = kSplitItemName[0];
			}
		}

		private void txtItemName_Leave(object sender, EventArgs e)
		{
			if (m_kItem == null || txtItemName.Text.Length == 0)
			{
				return;
			}

			m_kItem.Name = txtItemName.Text;
		}
    }
}
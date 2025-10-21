using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.IO;
using System.Windows.Forms;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Dialogs
{
	/// <summary>
	/// Summary description for AddNewPaletteDlg.
	/// </summary>
	public class AddNewPaletteDlg : System.Windows.Forms.Form
	{
        #region Private Data
        private string[] m_bannedNames;
        private string m_strDir;
        private string m_strName;
        #endregion

        private System.Windows.Forms.Button m_btnCancel;
        private System.Windows.Forms.Button m_btnOK;
        private System.Windows.Forms.TextBox m_tbName;
        private System.Windows.Forms.Label m_lblName;
        private System.Windows.Forms.ErrorProvider m_errorProvider;
        private Button m_btnOpen;
        private IContainer components;

		public AddNewPaletteDlg(string[] bannedNames)
		{
            m_bannedNames = bannedNames;
			InitializeComponent();

		}

        public string PaletteName
        {
            get { return m_strName; }
            set { m_strName = value; }
        }

        public string DirectoryPath
        {
            get { return m_strDir; }
            set { m_strDir = value; }
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
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(AddNewPaletteDlg));
            this.m_btnCancel = new System.Windows.Forms.Button();
            this.m_btnOK = new System.Windows.Forms.Button();
            this.m_tbName = new System.Windows.Forms.TextBox();
            this.m_lblName = new System.Windows.Forms.Label();
            this.m_errorProvider = new System.Windows.Forms.ErrorProvider(this.components);
            this.m_btnOpen = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.m_errorProvider)).BeginInit();
            this.SuspendLayout();
            // 
            // m_btnCancel
            // 
            this.m_btnCancel.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.m_btnCancel.CausesValidation = false;
            this.m_btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_btnCancel.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnCancel.Location = new System.Drawing.Point(191, 60);
            this.m_btnCancel.Name = "m_btnCancel";
            this.m_btnCancel.Size = new System.Drawing.Size(90, 25);
            this.m_btnCancel.TabIndex = 4;
            this.m_btnCancel.Text = "Cancel";
            // 
            // m_btnOK
            // 
            this.m_btnOK.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.m_btnOK.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnOK.Location = new System.Drawing.Point(85, 60);
            this.m_btnOK.Name = "m_btnOK";
            this.m_btnOK.Size = new System.Drawing.Size(90, 25);
            this.m_btnOK.TabIndex = 3;
            this.m_btnOK.Text = "OK";
            this.m_btnOK.Click += new System.EventHandler(this.m_btnOK_Click);
            // 
            // m_tbName
            // 
            this.m_tbName.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_errorProvider.SetIconAlignment(this.m_tbName, System.Windows.Forms.ErrorIconAlignment.BottomRight);
            this.m_tbName.Location = new System.Drawing.Point(12, 26);
            this.m_tbName.Name = "m_tbName";
            this.m_tbName.Size = new System.Drawing.Size(301, 21);
            this.m_tbName.TabIndex = 2;
            this.m_tbName.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.m_tbName_KeyPress);
            this.m_tbName.Validating += new System.ComponentModel.CancelEventHandler(this.m_tbName_Validating);
            // 
            // m_lblName
            // 
            this.m_lblName.AutoSize = true;
            this.m_lblName.Location = new System.Drawing.Point(12, 9);
            this.m_lblName.Name = "m_lblName";
            this.m_lblName.Size = new System.Drawing.Size(59, 12);
            this.m_lblName.TabIndex = 0;
            this.m_lblName.Text = "Directory:";
            // 
            // m_errorProvider
            // 
            this.m_errorProvider.ContainerControl = this;
            // 
            // m_btnOpen
            // 
            this.m_btnOpen.Location = new System.Drawing.Point(312, 26);
            this.m_btnOpen.Name = "m_btnOpen";
            this.m_btnOpen.Size = new System.Drawing.Size(30, 21);
            this.m_btnOpen.TabIndex = 1;
            this.m_btnOpen.Text = "...";
            this.m_btnOpen.UseVisualStyleBackColor = true;
            this.m_btnOpen.Click += new System.EventHandler(this.m_btnOpen_Click);
            // 
            // AddNewPaletteDlg
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(6, 14);
            this.ClientSize = new System.Drawing.Size(360, 104);
            this.Controls.Add(this.m_btnOpen);
            this.Controls.Add(this.m_tbName);
            this.Controls.Add(this.m_btnCancel);
            this.Controls.Add(this.m_btnOK);
            this.Controls.Add(this.m_lblName);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.MinimumSize = new System.Drawing.Size(221, 129);
            this.Name = "AddNewPaletteDlg";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Add New Palette";
            ((System.ComponentModel.ISupportInitialize)(this.m_errorProvider)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }
		#endregion

        private void m_tbName_Validating(object sender,
            System.ComponentModel.CancelEventArgs e)
        {
            return; 
            string proposedName = m_tbName.Text;
            Control control = sender as Control;
            if (proposedName.Equals(string.Empty))
            {
                m_errorProvider.SetError(control, 
                    "You must enter a name for the palette");
            }
            else if (Array.IndexOf(m_bannedNames, proposedName) != 
                m_bannedNames.GetLowerBound(0) - 1 )
            {
                m_errorProvider.SetError(control,
                    "A palette with that name already exists.");
            }
            else if (!IsValidFileName(proposedName))
            {
                m_errorProvider.SetError(control,
                    "The name is not a valid filename.");
            }
            else
            {
                m_errorProvider.SetError(control, "");
            }
        }


	    private void m_tbName_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
        {
            //m_tbName_Validating(m_tbName, new CancelEventArgs(false));
        }

        private void m_btnOK_Click(object sender, System.EventArgs e)
        {
            if (m_strDir == null)
            {
                MessageBox.Show("Invalid Input Value.");
                m_tbName.Focus();

                return;
            }

            DirectoryInfo DI = new DirectoryInfo(m_strDir);
            if (DI.Exists)
            {
                this.DialogResult = System.Windows.Forms.DialogResult.OK;
            }
            else
            {
                MessageBox.Show("Directory isn't exist.");
                m_tbName.Focus();
            }




            //string nameErrorString = m_errorProvider.GetError(m_tbName);
            //if (!nameErrorString.Equals(string.Empty))
            //{
            //    MessageBox.Show(nameErrorString, "Invalid Input Value");
            //    m_tbName.Focus();
            //}
            //else
            {
            }
        }

        private bool IsValidFileName(string name)
        {
            try
            {
                FileInfo fi = new FileInfo(name);
            }
            catch (ArgumentException)
            {
                return false;
            }
            if (name.IndexOf(Path.DirectorySeparatorChar) != -1
                || name.IndexOf(Path.AltDirectorySeparatorChar) != -1)
            {
                return false;
            }
            return true;
        }

        private void m_btnOpen_Click(object sender, EventArgs e)
        {
            SaveFileDialog dlg = new SaveFileDialog();
            string streamingDescriptions = "Palette files (*.pal)|*.pal";
            string streamingExtensions = "PAL";

            dlg.Filter = streamingDescriptions;
            if (dlg.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                string fileName = dlg.FileName;
                string extension = streamingExtensions;

                GetDirectoryNFilename(fileName);

                m_tbName.AppendText(fileName);
            }
        }

        private void GetDirectoryNFilename(string strFullFileName)
        {
            int iSrcIdx = strFullFileName.LastIndexOf("\\");
            int iLength = strFullFileName.LastIndexOf(".");
            //char[] szDest = new char[256];
            //string strDest = null;
            //strFullFileName.CopyTo(iSrcIdx + 1, szDest, 0, iLength - iSrcIdx - 1);
            m_strName = strFullFileName.Substring(iSrcIdx + 1, iLength - iSrcIdx - 1);

            //foreach (char c in szDest)
            //    if (c.ToString() != "\0")
            //        strDest += c.ToString();
            //m_strName = strDest;

            m_strDir = strFullFileName.Substring(0, iSrcIdx + 1);
            //strFullFileName.CopyTo(0, szDest, 0, iSrcIdx + 1);
            //foreach (char c in szDest)
            //    if (c.ToString() != "\0")
            //        strDest += c.ToString();
            //m_strDir = strDest;
        }
    }
}

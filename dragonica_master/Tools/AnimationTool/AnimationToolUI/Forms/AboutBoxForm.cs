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
	public class AboutBoxForm : System.Windows.Forms.Form
	{
        private System.Windows.Forms.Button btnOK;
        private System.Windows.Forms.PictureBox pictNDLLogo;
        private System.Windows.Forms.Label lblAboutText;
        private System.Windows.Forms.LinkLabel lnklblNDLWebsite;
        private Label label1;

        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.Container components = null;

		public AboutBoxForm()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(AboutBoxForm));
            this.btnOK = new System.Windows.Forms.Button();
            this.pictNDLLogo = new System.Windows.Forms.PictureBox();
            this.lblAboutText = new System.Windows.Forms.Label();
            this.lnklblNDLWebsite = new System.Windows.Forms.LinkLabel();
            this.label1 = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.pictNDLLogo)).BeginInit();
            this.SuspendLayout();
            // 
            // btnOK
            // 
            this.btnOK.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.btnOK.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnOK.Location = new System.Drawing.Point(231, 241);
            this.btnOK.Name = "btnOK";
            this.btnOK.Size = new System.Drawing.Size(87, 35);
            this.btnOK.TabIndex = 0;
            this.btnOK.Text = "OK";
            this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
            // 
            // pictNDLLogo
            // 
            this.pictNDLLogo.Image = ((System.Drawing.Image)(resources.GetObject("pictNDLLogo.Image")));
            this.pictNDLLogo.Location = new System.Drawing.Point(29, 17);
            this.pictNDLLogo.Name = "pictNDLLogo";
            this.pictNDLLogo.Size = new System.Drawing.Size(32, 32);
            this.pictNDLLogo.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.pictNDLLogo.TabIndex = 1;
            this.pictNDLLogo.TabStop = false;
            // 
            // lblAboutText
            // 
            this.lblAboutText.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.lblAboutText.Location = new System.Drawing.Point(75, 17);
            this.lblAboutText.Name = "lblAboutText";
            this.lblAboutText.Size = new System.Drawing.Size(461, 164);
            this.lblAboutText.TabIndex = 1;
            this.lblAboutText.Text = resources.GetString("lblAboutText.Text");
            // 
            // lnklblNDLWebsite
            // 
            this.lnklblNDLWebsite.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.lnklblNDLWebsite.AutoSize = true;
            this.lnklblNDLWebsite.Location = new System.Drawing.Point(75, 190);
            this.lnklblNDLWebsite.Name = "lnklblNDLWebsite";
            this.lnklblNDLWebsite.Size = new System.Drawing.Size(149, 12);
            this.lnklblNDLWebsite.TabIndex = 2;
            this.lnklblNDLWebsite.TabStop = true;
            this.lnklblNDLWebsite.Text = "http://www.emergent.net";
            this.lnklblNDLWebsite.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.lnklblNDLWebsite_LinkClicked);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(410, 272);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(126, 12);
            this.label1.TabIndex = 3;
            this.label1.Text = "KFM Version : 2.2.0.2";
            // 
            // AboutBoxForm
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(6, 14);
            this.ClientSize = new System.Drawing.Size(548, 293);
            this.ControlBox = false;
            this.Controls.Add(this.label1);
            this.Controls.Add(this.lnklblNDLWebsite);
            this.Controls.Add(this.lblAboutText);
            this.Controls.Add(this.pictNDLLogo);
            this.Controls.Add(this.btnOK);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Name = "AboutBoxForm";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "About Gamebryo Animation Tool";
            this.Load += new System.EventHandler(this.AboutBoxForm_Load);
            ((System.ComponentModel.ISupportInitialize)(this.pictNDLLogo)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }
		#endregion

        private void btnOK_Click(object sender, System.EventArgs e)
        {
            this.Close();
        }

        private void lnklblNDLWebsite_LinkClicked(object sender,
            System.Windows.Forms.LinkLabelLinkClickedEventArgs e)
        {
            System.Diagnostics.Process.Start(lnklblNDLWebsite.Text);
        }

        private void AboutBoxForm_Load(object sender, System.EventArgs e)
        {
            string strGamebryoVersion = MUtility.GetGamebryoVersion();
            string strBuildDate = MUtility.GetGamebryoBuildDate();
            string strVersionAndDate = strGamebryoVersion + " (" +
                strBuildDate + ")";

            string strText = lblAboutText.Text;
            strText = strText.Replace("%GAMEBRYO_SDK_VERSION_STRING%", 
                strVersionAndDate);
            lblAboutText.Text = strText;
        }
	}
}

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

using Emergent.Gamebryo.SceneDesigner.Framework;

namespace Emergent.Gamebryo.SceneDesigner.GUI
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
			this.Text = "About " + MFramework.Instance.ApplicationName;
			this.lblAboutText.Text = MFramework.Instance.ApplicationName + @"
For use with Gamebryo %GAMEBRYO_SDK_VERSION_STRING%

Copyright ?1996-2006 Emergent Game Technologies
All Rights Reserved

This software is supplied under the terms of a license agreement or nondisclosure agreement with Emergent Game Technologies and may not be copied or disclosed except in accordance with the terms of that agreement.

Emergent Game Technologies, Chapel Hill, North Carolina 27517
";

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
			((System.ComponentModel.ISupportInitialize)(this.pictNDLLogo)).BeginInit();
			this.SuspendLayout();
			// 
			// btnOK
			// 
			this.btnOK.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
			this.btnOK.FlatStyle = System.Windows.Forms.FlatStyle.System;
			this.btnOK.Location = new System.Drawing.Point(234, 213);
			this.btnOK.Name = "btnOK";
			this.btnOK.Size = new System.Drawing.Size(87, 34);
			this.btnOK.TabIndex = 0;
			this.btnOK.Text = "OK";
			this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
			// 
			// pictNDLLogo
			// 
			this.pictNDLLogo.BackColor = System.Drawing.Color.White;
			this.pictNDLLogo.Image = ((System.Drawing.Image)(resources.GetObject("pictNDLLogo.Image")));
			this.pictNDLLogo.Location = new System.Drawing.Point(34, 60);
			this.pictNDLLogo.Name = "pictNDLLogo";
			this.pictNDLLogo.Size = new System.Drawing.Size(128, 64);
			this.pictNDLLogo.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
			this.pictNDLLogo.TabIndex = 1;
			this.pictNDLLogo.TabStop = false;
			// 
			// lblAboutText
			// 
			this.lblAboutText.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.lblAboutText.Location = new System.Drawing.Point(230, 17);
			this.lblAboutText.Name = "lblAboutText";
			this.lblAboutText.Size = new System.Drawing.Size(312, 161);
			this.lblAboutText.TabIndex = 1;
			// 
			// lnklblNDLWebsite
			// 
			this.lnklblNDLWebsite.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.lnklblNDLWebsite.AutoSize = true;
			this.lnklblNDLWebsite.Location = new System.Drawing.Point(230, 187);
			this.lnklblNDLWebsite.Name = "lnklblNDLWebsite";
			this.lnklblNDLWebsite.Size = new System.Drawing.Size(149, 12);
			this.lnklblNDLWebsite.TabIndex = 2;
			this.lnklblNDLWebsite.TabStop = true;
			this.lnklblNDLWebsite.Text = "http://www.emergent.net";
			this.lnklblNDLWebsite.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.lnklblNDLWebsite_LinkClicked);
			// 
			// AboutBoxForm
			// 
			this.AcceptButton = this.btnOK;
			this.AutoScaleBaseSize = new System.Drawing.Size(6, 14);
			this.BackColor = System.Drawing.Color.White;
			this.ClientSize = new System.Drawing.Size(554, 256);
			this.Controls.Add(this.lnklblNDLWebsite);
			this.Controls.Add(this.lblAboutText);
			this.Controls.Add(this.pictNDLLogo);
			this.Controls.Add(this.btnOK);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "AboutBoxForm";
			this.ShowInTaskbar = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "About";
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

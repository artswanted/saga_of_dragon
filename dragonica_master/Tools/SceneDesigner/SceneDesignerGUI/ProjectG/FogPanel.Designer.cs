namespace Emergent.Gamebryo.SceneDesigner.GUI.ProjectG
{
	partial class FogPanel
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
			this.m_trbNear = new System.Windows.Forms.TrackBar();
			this.m_btnFogColor = new System.Windows.Forms.Button();
			this.m_trbFar = new System.Windows.Forms.TrackBar();
			this.m_trbDensity = new System.Windows.Forms.TrackBar();
			this.label1 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.label3 = new System.Windows.Forms.Label();
			this.m_cbFogOnOff = new System.Windows.Forms.CheckBox();
			this.label4 = new System.Windows.Forms.Label();
			this.m_rbZLinear = new System.Windows.Forms.RadioButton();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.m_rbRangeRQ = new System.Windows.Forms.RadioButton();
			this.m_tbResult = new System.Windows.Forms.TextBox();
			this.label5 = new System.Windows.Forms.Label();
			this.m_btnApplyFog = new System.Windows.Forms.Button();
			((System.ComponentModel.ISupportInitialize)(this.m_trbNear)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.m_trbFar)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.m_trbDensity)).BeginInit();
			this.groupBox1.SuspendLayout();
			this.SuspendLayout();
			// 
			// m_trbNear
			// 
			this.m_trbNear.LargeChange = 1;
			this.m_trbNear.Location = new System.Drawing.Point(60, 130);
			this.m_trbNear.Maximum = 2000;
			this.m_trbNear.Name = "m_trbNear";
			this.m_trbNear.Size = new System.Drawing.Size(159, 45);
			this.m_trbNear.TabIndex = 0;
			this.m_trbNear.TabStop = false;
			this.m_trbNear.TickFrequency = 1000;
			this.m_trbNear.MouseUp += new System.Windows.Forms.MouseEventHandler(this.m_trbNear_MouseUp);
			// 
			// m_btnFogColor
			// 
			this.m_btnFogColor.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnFogColor.Font = new System.Drawing.Font("Gulim", 9F);
			this.m_btnFogColor.Location = new System.Drawing.Point(12, 40);
			this.m_btnFogColor.Name = "m_btnFogColor";
			this.m_btnFogColor.RightToLeft = System.Windows.Forms.RightToLeft.No;
			this.m_btnFogColor.Size = new System.Drawing.Size(206, 26);
			this.m_btnFogColor.TabIndex = 1;
			this.m_btnFogColor.Text = "안개 색상";
			this.m_btnFogColor.UseVisualStyleBackColor = true;
			this.m_btnFogColor.Click += new System.EventHandler(this.m_btnFogColor_Click);
			// 
			// m_trbFar
			// 
			this.m_trbFar.LargeChange = 1;
			this.m_trbFar.Location = new System.Drawing.Point(60, 173);
			this.m_trbFar.Maximum = 2000;
			this.m_trbFar.Name = "m_trbFar";
			this.m_trbFar.Size = new System.Drawing.Size(159, 45);
			this.m_trbFar.TabIndex = 2;
			this.m_trbFar.TabStop = false;
			this.m_trbFar.TickFrequency = 1000;
			this.m_trbFar.MouseUp += new System.Windows.Forms.MouseEventHandler(this.m_trbFar_MouseUp);
			// 
			// m_trbDensity
			// 
			this.m_trbDensity.LargeChange = 1;
			this.m_trbDensity.Location = new System.Drawing.Point(60, 216);
			this.m_trbDensity.Maximum = 100;
			this.m_trbDensity.Name = "m_trbDensity";
			this.m_trbDensity.Size = new System.Drawing.Size(159, 45);
			this.m_trbDensity.TabIndex = 3;
			this.m_trbDensity.TabStop = false;
			this.m_trbDensity.TickFrequency = 10;
			this.m_trbDensity.MouseUp += new System.Windows.Forms.MouseEventHandler(this.m_trbDensity_MouseUp);
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(13, 137);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(41, 12);
			this.label1.TabIndex = 4;
			this.label1.Text = "근단면";
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(13, 180);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(41, 12);
			this.label2.TabIndex = 5;
			this.label2.Text = "원단면";
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Location = new System.Drawing.Point(13, 223);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(41, 12);
			this.label3.TabIndex = 6;
			this.label3.Text = "강   도";
			// 
			// m_cbFogOnOff
			// 
			this.m_cbFogOnOff.AutoSize = true;
			this.m_cbFogOnOff.Location = new System.Drawing.Point(96, 12);
			this.m_cbFogOnOff.Name = "m_cbFogOnOff";
			this.m_cbFogOnOff.Size = new System.Drawing.Size(15, 14);
			this.m_cbFogOnOff.TabIndex = 7;
			this.m_cbFogOnOff.UseVisualStyleBackColor = true;
			this.m_cbFogOnOff.CheckedChanged += new System.EventHandler(this.m_cbFogOnOff_CheckedChanged);
			// 
			// label4
			// 
			this.label4.AutoSize = true;
			this.label4.Location = new System.Drawing.Point(10, 13);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(53, 12);
			this.label4.TabIndex = 8;
			this.label4.Text = "안개켜기";
			// 
			// m_rbZLinear
			// 
			this.m_rbZLinear.AutoSize = true;
			this.m_rbZLinear.Location = new System.Drawing.Point(6, 20);
			this.m_rbZLinear.Name = "m_rbZLinear";
			this.m_rbZLinear.Size = new System.Drawing.Size(80, 16);
			this.m_rbZLinear.TabIndex = 9;
			this.m_rbZLinear.TabStop = true;
			this.m_rbZLinear.Text = "Z_LINEAR";
			this.m_rbZLinear.UseVisualStyleBackColor = true;
			this.m_rbZLinear.CheckedChanged += new System.EventHandler(this.m_rbZLinear_CheckedChanged);
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.m_rbRangeRQ);
			this.groupBox1.Controls.Add(this.m_rbZLinear);
			this.groupBox1.Location = new System.Drawing.Point(12, 72);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(207, 49);
			this.groupBox1.TabIndex = 10;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "안개 형식";
			// 
			// m_rbRangeRQ
			// 
			this.m_rbRangeRQ.AutoSize = true;
			this.m_rbRangeRQ.Location = new System.Drawing.Point(100, 20);
			this.m_rbRangeRQ.Name = "m_rbRangeRQ";
			this.m_rbRangeRQ.Size = new System.Drawing.Size(88, 16);
			this.m_rbRangeRQ.TabIndex = 10;
			this.m_rbRangeRQ.TabStop = true;
			this.m_rbRangeRQ.Text = "RANGE_SQ";
			this.m_rbRangeRQ.UseVisualStyleBackColor = true;
			this.m_rbRangeRQ.CheckedChanged += new System.EventHandler(this.m_rbRangeRQ_CheckedChanged);
			// 
			// m_tbResult
			// 
			this.m_tbResult.Location = new System.Drawing.Point(60, 258);
			this.m_tbResult.Name = "m_tbResult";
			this.m_tbResult.ReadOnly = true;
			this.m_tbResult.Size = new System.Drawing.Size(158, 21);
			this.m_tbResult.TabIndex = 11;
			// 
			// label5
			// 
			this.label5.AutoSize = true;
			this.label5.Location = new System.Drawing.Point(13, 261);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(41, 12);
			this.label5.TabIndex = 12;
			this.label5.Text = "결   과";
			// 
			// m_btnApplyFog
			// 
			this.m_btnApplyFog.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnApplyFog.Location = new System.Drawing.Point(15, 288);
			this.m_btnApplyFog.Name = "m_btnApplyFog";
			this.m_btnApplyFog.Size = new System.Drawing.Size(202, 23);
			this.m_btnApplyFog.TabIndex = 13;
			this.m_btnApplyFog.Text = "포그 적용";
			this.m_btnApplyFog.UseVisualStyleBackColor = true;
			this.m_btnApplyFog.Click += new System.EventHandler(this.m_btnApplyFog_Click);
			// 
			// FogPanel
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.AutoScroll = true;
			this.ClientSize = new System.Drawing.Size(231, 355);
			this.Controls.Add(this.m_btnApplyFog);
			this.Controls.Add(this.label5);
			this.Controls.Add(this.m_tbResult);
			this.Controls.Add(this.groupBox1);
			this.Controls.Add(this.label4);
			this.Controls.Add(this.m_cbFogOnOff);
			this.Controls.Add(this.label3);
			this.Controls.Add(this.label2);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.m_trbDensity);
			this.Controls.Add(this.m_trbFar);
			this.Controls.Add(this.m_btnFogColor);
			this.Controls.Add(this.m_trbNear);
			this.Name = "FogPanel";
			this.Text = "FogPanel";
			((System.ComponentModel.ISupportInitialize)(this.m_trbNear)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.m_trbFar)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.m_trbDensity)).EndInit();
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.TrackBar m_trbNear;
		private System.Windows.Forms.Button m_btnFogColor;
		private System.Windows.Forms.TrackBar m_trbFar;
		private System.Windows.Forms.TrackBar m_trbDensity;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.CheckBox m_cbFogOnOff;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.RadioButton m_rbZLinear;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.RadioButton m_rbRangeRQ;
		private System.Windows.Forms.TextBox m_tbResult;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.Button m_btnApplyFog;
	}
}
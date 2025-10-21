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
using System.Xml;

namespace AnimationToolUI
{
	/// <summary>
	/// Summary description for HelpForm.
	/// </summary>
	public class TextKeyForm : System.Windows.Forms.Form
	{
		private Label label1;
		private Label label3;
		private Label label2;
		private Label label4;
		private Label label5;
		private Label label6;
		private Label label7;
		private Label label8;
		private Label label9;
		private Label label10;
		private Label label11;
		
		private ComboBox cbEventType;
		private ComboBox cbAttachPoint;
		
		private GroupBox gbBase;
		private GroupBox gbEffect;
		private GroupBox gbSound;
		
		private TextBox txtEffectName;
		private TextBox txtTime;
		private TextBox txtSoundName;
		private TextBox txtTextKey;

		private Button btnAdd;
		private Button btnCancel;
		private Button btnBrowseEffect;
		private Button btnBrowseSound;
		
		private CheckBox ckAttach;
		private CheckBox ckLoop;

		private NumericSlider nsEffectScale;
		private NumericSlider nsMaxDistance;
		private NumericSlider nsMinDistance;
		private NumericSlider nsVolume;
		
		private MSequence.MTextKey m_kTextKeyInfo = null;
        private Button Btn_RemoveAttachPoint;
        private Button Btn_AddAttachPoint;

        private ToolTip ttAttach;

		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public TextKeyForm()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

            ttAttach = new ToolTip();
            ttAttach.SetToolTip(ckAttach, "이펙트가 월드에 Attach 됩니다.");
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

        private void InitializeAttachPointList()
        {
            this.cbAttachPoint.Items.Clear();
            uint uiNumAttachPoint = MFramework.Instance.AttachPointList.GetNumAttachPoint();
            for (uint ui = 0; ui < uiNumAttachPoint; ++ui)
            {
                this.cbAttachPoint.Items.Add(MFramework.Instance.AttachPointList.GetAttachPointAt(ui));
            }
            this.Btn_AddAttachPoint.Hide();
            this.Btn_RemoveAttachPoint.Hide();
        }

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
        /// 


		private void InitializeComponent()
		{
            this.cbEventType = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.gbBase = new System.Windows.Forms.GroupBox();
            this.txtTextKey = new System.Windows.Forms.TextBox();
            this.txtTime = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.txtEffectName = new System.Windows.Forms.TextBox();
            this.btnAdd = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.gbEffect = new System.Windows.Forms.GroupBox();
            this.Btn_RemoveAttachPoint = new System.Windows.Forms.Button();
            this.Btn_AddAttachPoint = new System.Windows.Forms.Button();
            this.ckLoop = new System.Windows.Forms.CheckBox();
            this.ckAttach = new System.Windows.Forms.CheckBox();
            this.btnBrowseEffect = new System.Windows.Forms.Button();
            this.label6 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.cbAttachPoint = new System.Windows.Forms.ComboBox();
            this.label4 = new System.Windows.Forms.Label();
            this.nsEffectScale = new AnimationToolUI.NumericSlider();
            this.gbSound = new System.Windows.Forms.GroupBox();
            this.label11 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.txtSoundName = new System.Windows.Forms.TextBox();
            this.btnBrowseSound = new System.Windows.Forms.Button();
            this.nsMaxDistance = new AnimationToolUI.NumericSlider();
            this.nsMinDistance = new AnimationToolUI.NumericSlider();
            this.nsVolume = new AnimationToolUI.NumericSlider();
            this.gbBase.SuspendLayout();
            this.gbEffect.SuspendLayout();
            this.gbSound.SuspendLayout();
            this.SuspendLayout();
            // 
            // cbEventType
            // 
            this.cbEventType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbEventType.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.cbEventType.FormattingEnabled = true;
            this.cbEventType.Items.AddRange(new object[] {
            "TextKey Event",
            "Effect Event",
            "Sound Event"});
            this.cbEventType.Location = new System.Drawing.Point(93, 16);
            this.cbEventType.Name = "cbEventType";
            this.cbEventType.Size = new System.Drawing.Size(158, 21);
            this.cbEventType.TabIndex = 2;
            this.cbEventType.SelectedIndexChanged += new System.EventHandler(this.cbEventType_SelectedIndexChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(15, 21);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(31, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Type";
            // 
            // gbBase
            // 
            this.gbBase.Controls.Add(this.txtTextKey);
            this.gbBase.Controls.Add(this.txtTime);
            this.gbBase.Controls.Add(this.label7);
            this.gbBase.Controls.Add(this.label3);
            this.gbBase.Controls.Add(this.label1);
            this.gbBase.Controls.Add(this.cbEventType);
            this.gbBase.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.gbBase.Location = new System.Drawing.Point(12, 12);
            this.gbBase.Name = "gbBase";
            this.gbBase.Size = new System.Drawing.Size(271, 105);
            this.gbBase.TabIndex = 0;
            this.gbBase.TabStop = false;
            this.gbBase.Text = "Base";
            // 
            // txtTextKey
            // 
            this.txtTextKey.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtTextKey.Location = new System.Drawing.Point(93, 45);
            this.txtTextKey.Name = "txtTextKey";
            this.txtTextKey.Size = new System.Drawing.Size(158, 21);
            this.txtTextKey.TabIndex = 3;
            // 
            // txtTime
            // 
            this.txtTime.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtTime.Location = new System.Drawing.Point(93, 71);
            this.txtTime.Name = "txtTime";
            this.txtTime.Size = new System.Drawing.Size(158, 21);
            this.txtTime.TabIndex = 4;
            this.txtTime.Leave += new System.EventHandler(this.txtTime_Leave);
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label7.Location = new System.Drawing.Point(15, 48);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(47, 13);
            this.label7.TabIndex = 1;
            this.label7.Text = "TextKey";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label3.Location = new System.Drawing.Point(15, 75);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(29, 13);
            this.label3.TabIndex = 1;
            this.label3.Text = "Time";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label2.Location = new System.Drawing.Point(15, 21);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(34, 13);
            this.label2.TabIndex = 1;
            this.label2.Text = "Name";
            // 
            // txtEffectName
            // 
            this.txtEffectName.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtEffectName.Location = new System.Drawing.Point(92, 16);
            this.txtEffectName.Name = "txtEffectName";
            this.txtEffectName.Size = new System.Drawing.Size(127, 21);
            this.txtEffectName.TabIndex = 2;
            // 
            // btnAdd
            // 
            this.btnAdd.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btnAdd.Location = new System.Drawing.Point(65, 416);
            this.btnAdd.Name = "btnAdd";
            this.btnAdd.Size = new System.Drawing.Size(75, 23);
            this.btnAdd.TabIndex = 3;
            this.btnAdd.Text = "&Add";
            this.btnAdd.UseVisualStyleBackColor = true;
            this.btnAdd.Click += new System.EventHandler(this.btnAdd_Click);
            this.btnAdd.MouseDown += new System.Windows.Forms.MouseEventHandler(this.OnAddBtnDown);
            // 
            // btnCancel
            // 
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btnCancel.Location = new System.Drawing.Point(159, 416);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(75, 23);
            this.btnCancel.TabIndex = 4;
            this.btnCancel.Text = "&Cancel";
            this.btnCancel.UseVisualStyleBackColor = true;
            this.btnCancel.MouseDown += new System.Windows.Forms.MouseEventHandler(this.OnCanelBtnDown);
            // 
            // gbEffect
            // 
            this.gbEffect.Controls.Add(this.Btn_RemoveAttachPoint);
            this.gbEffect.Controls.Add(this.Btn_AddAttachPoint);
            this.gbEffect.Controls.Add(this.ckLoop);
            this.gbEffect.Controls.Add(this.ckAttach);
            this.gbEffect.Controls.Add(this.btnBrowseEffect);
            this.gbEffect.Controls.Add(this.label6);
            this.gbEffect.Controls.Add(this.label5);
            this.gbEffect.Controls.Add(this.cbAttachPoint);
            this.gbEffect.Controls.Add(this.label4);
            this.gbEffect.Controls.Add(this.label2);
            this.gbEffect.Controls.Add(this.nsEffectScale);
            this.gbEffect.Controls.Add(this.txtEffectName);
            this.gbEffect.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.gbEffect.Location = new System.Drawing.Point(13, 123);
            this.gbEffect.Name = "gbEffect";
            this.gbEffect.Size = new System.Drawing.Size(270, 131);
            this.gbEffect.TabIndex = 2;
            this.gbEffect.TabStop = false;
            this.gbEffect.Text = "Effect";
            this.gbEffect.EnabledChanged += new System.EventHandler(this.gbEffect_EnabledChanged);
            // 
            // Btn_RemoveAttachPoint
            // 
            this.Btn_RemoveAttachPoint.Location = new System.Drawing.Point(250, 43);
            this.Btn_RemoveAttachPoint.Name = "Btn_RemoveAttachPoint";
            this.Btn_RemoveAttachPoint.Size = new System.Drawing.Size(20, 23);
            this.Btn_RemoveAttachPoint.TabIndex = 8;
            this.Btn_RemoveAttachPoint.Text = "-";
            this.Btn_RemoveAttachPoint.UseVisualStyleBackColor = true;
            this.Btn_RemoveAttachPoint.MouseDown += new System.Windows.Forms.MouseEventHandler(this.OnBtnRemovedAttachPointDown);
            // 
            // Btn_AddAttachPoint
            // 
            this.Btn_AddAttachPoint.Location = new System.Drawing.Point(250, 43);
            this.Btn_AddAttachPoint.Name = "Btn_AddAttachPoint";
            this.Btn_AddAttachPoint.Size = new System.Drawing.Size(20, 23);
            this.Btn_AddAttachPoint.TabIndex = 8;
            this.Btn_AddAttachPoint.Text = "+";
            this.Btn_AddAttachPoint.UseVisualStyleBackColor = true;
            this.Btn_AddAttachPoint.MouseDown += new System.Windows.Forms.MouseEventHandler(this.OnBtnAddAttachPointDown);
            // 
            // ckLoop
            // 
            this.ckLoop.AutoSize = true;
            this.ckLoop.Location = new System.Drawing.Point(170, 103);
            this.ckLoop.Name = "ckLoop";
            this.ckLoop.Size = new System.Drawing.Size(49, 17);
            this.ckLoop.TabIndex = 7;
            this.ckLoop.Text = "Loop";
            this.ckLoop.UseVisualStyleBackColor = true;
            // 
            // ckAttach
            // 
            this.ckAttach.AutoSize = true;
            this.ckAttach.Location = new System.Drawing.Point(92, 103);
            this.ckAttach.Name = "ckAttach";
            this.ckAttach.Size = new System.Drawing.Size(60, 17);
            this.ckAttach.TabIndex = 6;
            this.ckAttach.Text = "Detach";
            this.ckAttach.UseVisualStyleBackColor = true;
            // 
            // btnBrowseEffect
            // 
            this.btnBrowseEffect.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btnBrowseEffect.Location = new System.Drawing.Point(223, 15);
            this.btnBrowseEffect.Name = "btnBrowseEffect";
            this.btnBrowseEffect.Size = new System.Drawing.Size(27, 23);
            this.btnBrowseEffect.TabIndex = 3;
            this.btnBrowseEffect.Text = "...";
            this.btnBrowseEffect.UseVisualStyleBackColor = true;
            this.btnBrowseEffect.Click += new System.EventHandler(this.btnBrowseEffect_Click);
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label6.Location = new System.Drawing.Point(15, 103);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(47, 13);
            this.label6.TabIndex = 1;
            this.label6.Text = "Optional";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label5.Location = new System.Drawing.Point(15, 75);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(32, 13);
            this.label5.TabIndex = 1;
            this.label5.Text = "Scale";
            // 
            // cbAttachPoint
            // 
            this.cbAttachPoint.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.cbAttachPoint.FormattingEnabled = true;
            this.cbAttachPoint.Items.AddRange(new object[] {
            "char_root",
            "p_ef_center",
            "p_ef_head",
            "p_ef_heart",
            "p_ef_star",
            "p_pt_back",
            "p_pt_chest",
            "p_pt_hair",
            "p_pt_ride"});
            this.cbAttachPoint.Location = new System.Drawing.Point(92, 43);
            this.cbAttachPoint.Name = "cbAttachPoint";
            this.cbAttachPoint.Size = new System.Drawing.Size(158, 21);
            this.cbAttachPoint.Sorted = true;
            this.cbAttachPoint.TabIndex = 4;
            this.cbAttachPoint.SelectedIndexChanged += new System.EventHandler(this.OnAttachPointSelectionChanged);
            this.cbAttachPoint.TextChanged += new System.EventHandler(this.OnAttachPointTextChanged);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label4.Location = new System.Drawing.Point(15, 48);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(66, 13);
            this.label4.TabIndex = 1;
            this.label4.Text = "Attach Point";
            // 
            // nsEffectScale
            // 
            this.nsEffectScale.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.nsEffectScale.DecimalPlaces = 2;
            this.nsEffectScale.EnableMeter = true;
            this.nsEffectScale.EnablePopup = false;
            this.nsEffectScale.Increment = new decimal(new int[] {
            100,
            0,
            0,
            262144});
            this.nsEffectScale.Location = new System.Drawing.Point(92, 70);
            this.nsEffectScale.Maximum = new decimal(new int[] {
            10000,
            0,
            0,
            196608});
            this.nsEffectScale.MeterLeftColor = System.Drawing.SystemColors.ActiveCaption;
            this.nsEffectScale.MeterRightColor = System.Drawing.SystemColors.Control;
            this.nsEffectScale.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            196608});
            this.nsEffectScale.Name = "nsEffectScale";
            this.nsEffectScale.PopupHeight = 16;
            this.nsEffectScale.ReadOnly = false;
            this.nsEffectScale.Size = new System.Drawing.Size(158, 28);
            this.nsEffectScale.TabIndex = 5;
            this.nsEffectScale.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.nsEffectScale.Value = new decimal(new int[] {
            10,
            0,
            0,
            65536});
            // 
            // gbSound
            // 
            this.gbSound.Controls.Add(this.label11);
            this.gbSound.Controls.Add(this.label10);
            this.gbSound.Controls.Add(this.label9);
            this.gbSound.Controls.Add(this.label8);
            this.gbSound.Controls.Add(this.txtSoundName);
            this.gbSound.Controls.Add(this.btnBrowseSound);
            this.gbSound.Controls.Add(this.nsMaxDistance);
            this.gbSound.Controls.Add(this.nsMinDistance);
            this.gbSound.Controls.Add(this.nsVolume);
            this.gbSound.Location = new System.Drawing.Point(12, 260);
            this.gbSound.Name = "gbSound";
            this.gbSound.Size = new System.Drawing.Size(271, 145);
            this.gbSound.TabIndex = 2;
            this.gbSound.TabStop = false;
            this.gbSound.Text = "Sound";
            this.gbSound.EnabledChanged += new System.EventHandler(this.gbSound_EnabledChanged);
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label11.Location = new System.Drawing.Point(15, 110);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(71, 13);
            this.label11.TabIndex = 1;
            this.label11.Text = "Max Distance";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label10.Location = new System.Drawing.Point(15, 80);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(67, 13);
            this.label10.TabIndex = 1;
            this.label10.Text = "Min Distance";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label9.Location = new System.Drawing.Point(15, 47);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(41, 13);
            this.label9.TabIndex = 1;
            this.label9.Text = "Volume";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label8.Location = new System.Drawing.Point(15, 21);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(34, 13);
            this.label8.TabIndex = 1;
            this.label8.Text = "Name";
            // 
            // txtSoundName
            // 
            this.txtSoundName.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtSoundName.Location = new System.Drawing.Point(93, 17);
            this.txtSoundName.Name = "txtSoundName";
            this.txtSoundName.Size = new System.Drawing.Size(127, 21);
            this.txtSoundName.TabIndex = 2;
            // 
            // btnBrowseSound
            // 
            this.btnBrowseSound.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btnBrowseSound.Location = new System.Drawing.Point(226, 16);
            this.btnBrowseSound.Name = "btnBrowseSound";
            this.btnBrowseSound.Size = new System.Drawing.Size(27, 23);
            this.btnBrowseSound.TabIndex = 3;
            this.btnBrowseSound.Text = "...";
            this.btnBrowseSound.UseVisualStyleBackColor = true;
            this.btnBrowseSound.Click += new System.EventHandler(this.btnBrowseSound_Click);
            // 
            // nsMaxDistance
            // 
            this.nsMaxDistance.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.nsMaxDistance.DecimalPlaces = 1;
            this.nsMaxDistance.EnableMeter = true;
            this.nsMaxDistance.EnablePopup = false;
            this.nsMaxDistance.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.nsMaxDistance.Location = new System.Drawing.Point(93, 106);
            this.nsMaxDistance.Maximum = new decimal(new int[] {
            9999,
            0,
            0,
            0});
            this.nsMaxDistance.MeterLeftColor = System.Drawing.SystemColors.ActiveCaption;
            this.nsMaxDistance.MeterRightColor = System.Drawing.SystemColors.Control;
            this.nsMaxDistance.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.nsMaxDistance.Name = "nsMaxDistance";
            this.nsMaxDistance.PopupHeight = 16;
            this.nsMaxDistance.ReadOnly = false;
            this.nsMaxDistance.Size = new System.Drawing.Size(158, 28);
            this.nsMaxDistance.TabIndex = 6;
            this.nsMaxDistance.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.nsMaxDistance.Value = new decimal(new int[] {
            2500,
            0,
            0,
            65536});
            // 
            // nsMinDistance
            // 
            this.nsMinDistance.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.nsMinDistance.DecimalPlaces = 1;
            this.nsMinDistance.EnableMeter = true;
            this.nsMinDistance.EnablePopup = false;
            this.nsMinDistance.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.nsMinDistance.Location = new System.Drawing.Point(93, 74);
            this.nsMinDistance.Maximum = new decimal(new int[] {
            9999,
            0,
            0,
            0});
            this.nsMinDistance.MeterLeftColor = System.Drawing.SystemColors.ActiveCaption;
            this.nsMinDistance.MeterRightColor = System.Drawing.SystemColors.Control;
            this.nsMinDistance.Minimum = new decimal(new int[] {
            0,
            0,
            0,
            0});
            this.nsMinDistance.Name = "nsMinDistance";
            this.nsMinDistance.PopupHeight = 16;
            this.nsMinDistance.ReadOnly = false;
            this.nsMinDistance.Size = new System.Drawing.Size(158, 28);
            this.nsMinDistance.TabIndex = 5;
            this.nsMinDistance.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.nsMinDistance.Value = new decimal(new int[] {
            500,
            0,
            0,
            65536});
            // 
            // nsVolume
            // 
            this.nsVolume.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.nsVolume.DecimalPlaces = 2;
            this.nsVolume.EnableMeter = true;
            this.nsVolume.EnablePopup = false;
            this.nsVolume.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.nsVolume.Location = new System.Drawing.Point(93, 42);
            this.nsVolume.Maximum = new decimal(new int[] {
            100,
            0,
            0,
            65536});
            this.nsVolume.MeterLeftColor = System.Drawing.SystemColors.ActiveCaption;
            this.nsVolume.MeterRightColor = System.Drawing.SystemColors.Control;
            this.nsVolume.Minimum = new decimal(new int[] {
            0,
            0,
            0,
            65536});
            this.nsVolume.Name = "nsVolume";
            this.nsVolume.PopupHeight = 16;
            this.nsVolume.ReadOnly = false;
            this.nsVolume.Size = new System.Drawing.Size(158, 28);
            this.nsVolume.TabIndex = 4;
            this.nsVolume.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.nsVolume.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            // 
            // TextKeyForm
            // 
            this.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.ClientSize = new System.Drawing.Size(299, 450);
            this.Controls.Add(this.gbSound);
            this.Controls.Add(this.gbEffect);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.gbBase);
            this.Controls.Add(this.btnAdd);
            this.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Name = "TextKeyForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Add TextKey Events..";
            this.Load += new System.EventHandler(this.TextKeyForm_Load);
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.OnTextKeyFormClosed);
            this.gbBase.ResumeLayout(false);
            this.gbBase.PerformLayout();
            this.gbEffect.ResumeLayout(false);
            this.gbEffect.PerformLayout();
            this.gbSound.ResumeLayout(false);
            this.gbSound.PerformLayout();
            this.ResumeLayout(false);

		}
		#endregion


        private void SetAttachPointSelectedIndex(int uiIndex)
        {
            if (uiIndex >= cbAttachPoint.Items.Count)
            {
                return;
            }

            cbAttachPoint.SelectedIndex = uiIndex;
        }
        private void TextKeyForm_Load(object sender, System.EventArgs e)
		{
            InitializeAttachPointList();

            if (m_kTextKeyInfo == null)
			{
				cbEventType.SelectedIndex = 0;
                SetAttachPointSelectedIndex(0);
			}

		}

		public void SetEditTextKeyInfo(MSequence.MTextKey kTextKey)
		{
			m_kTextKeyInfo = kTextKey;

			if (kTextKey == null)
			{
				btnAdd.Text = "Add";
				return;
			}

			// Event Type은 바꿀 수 없다.
			cbEventType.Enabled = false;

			txtTextKey.Text = kTextKey.Text;
			txtTime.Text = kTextKey.Time.ToString("f3");
			cbEventType.SelectedIndex = 0;

			if (kTextKey.ExtraData is MSequence.MTextKey.MEffectData)
			{
				cbEventType.SelectedIndex = 1;
				MSequence.MTextKey.MEffectData kEffectData = (MSequence.MTextKey.MEffectData)kTextKey.ExtraData;
				txtEffectName.Text = kEffectData.EffectName;
                AddAttachPoint(kEffectData.AttachPoint);
                SetAttachPointSelectedIndex(cbAttachPoint.FindString(kEffectData.AttachPoint, 0));
                this.Btn_RemoveAttachPoint.Show();
				nsEffectScale.Value = new decimal(kEffectData.Scale);
				ckAttach.Checked = kEffectData.Attach;
				ckLoop.Checked = kEffectData.Loop;
			}
			else if (kTextKey.ExtraData is MSequence.MTextKey.MAudioData)
			{
				cbEventType.SelectedIndex = 2;
				MSequence.MTextKey.MAudioData kAudioData = (MSequence.MTextKey.MAudioData)kTextKey.ExtraData;
				txtSoundName.Text = kAudioData.SoundName;
				nsVolume.Value = new decimal(kAudioData.Volume);
				nsMaxDistance.Value = new decimal(kAudioData.MaxDistance);
				nsMinDistance.Value = new decimal(kAudioData.MinDistance);
			}

			btnAdd.Text = "Edit";
		}

		public void SetCurrentTime(string kCurrentTime)
		{
			txtTime.Text = kCurrentTime;
		}

		private void gbEffect_EnabledChanged(object sender, EventArgs e)
		{
			bool bEnabled = gbEffect.Enabled;
			this.txtEffectName.Enabled = bEnabled;
			this.cbAttachPoint.Enabled = bEnabled;
			this.ckAttach.Enabled = bEnabled;
			this.ckLoop.Enabled = bEnabled;
			this.nsEffectScale.Enabled = bEnabled;

			if (bEnabled)
			{
				txtEffectName.BackColor = Color.White;
			}
			else
			{
				txtEffectName.BackColor = Color.WhiteSmoke;
			}
		}

		private void gbSound_EnabledChanged(object sender, EventArgs e)
		{
			bool bEnabled = gbSound.Enabled;
			this.txtSoundName.Enabled = bEnabled;
			this.nsVolume.Enabled = bEnabled;
			this.nsMinDistance.Enabled = bEnabled;
			this.nsMaxDistance.Enabled = bEnabled;

			if (bEnabled)
			{
				txtSoundName.BackColor = Color.White;
				nsVolume.BackColor = Color.White;
			}
			else
			{
				txtSoundName.BackColor = Color.WhiteSmoke;
				nsVolume.BackColor = Color.WhiteSmoke;
			}
		}

		private void cbEventType_SelectedIndexChanged(object sender, EventArgs e)
		{
			if (cbEventType.Text == "TextKey Event")
			{
				gbSound.Enabled = false;
				gbEffect.Enabled = false;
                this.Btn_AddAttachPoint.Hide();
                this.Btn_RemoveAttachPoint.Hide();
			}
			else if (cbEventType.Text == "Effect Event")
			{
				gbSound.Enabled = false;
				gbEffect.Enabled = true;
                this.Btn_AddAttachPoint.Hide();
                this.Btn_RemoveAttachPoint.Show();
            }
			else if (cbEventType.Text == "Sound Event")
			{
				gbSound.Enabled = true;
				gbEffect.Enabled = false;
                this.Btn_AddAttachPoint.Hide();
                this.Btn_RemoveAttachPoint.Hide();
            }
		}

		private void btnAdd_Click(object sender, EventArgs e)
		{
			bool bRet = true;
			if (cbEventType.Text == "TextKey Event")
			{
				if (txtTextKey.Text.Trim().Length == 0)
				{
					txtTextKey.Focus();
					MessageBox.Show("TextKey Name을 입력해주세요");
					return;
				}

				// 수정해야 한다면, 기존 Event를 지우고 추가한다.
				if (m_kTextKeyInfo != null)
				{
					MFramework.Instance.Animation.RemoveEvent(m_kTextKeyInfo.Text, m_kTextKeyInfo.Time, 
						MAnimation.EventType.TextKeyEvent);
				}
				
				if (MFramework.Instance.Animation.AddTextKeyEvent(txtTextKey.Text.Trim(),
					float.Parse(txtTime.Text.Trim())))
				{
					DialogResult = DialogResult.OK;
				}
				else
				{
					bRet = false;
				}
			}
			else if (cbEventType.Text == "Effect Event")
			{
				if (txtTextKey.Text.Trim().Length == 0)
				{
					txtTextKey.Focus();
					MessageBox.Show("TextKey Name을 입력해주세요");
					return;
				}

				if (txtEffectName.Text.Trim().Length == 0)
				{
					txtEffectName.Focus();
					MessageBox.Show("Effect Symbol을 입력해주세요.");
					return;
				}

				// 수정해야 한다면, 기존 Event를 지우고 추가한다.
				if (m_kTextKeyInfo != null)
				{
					MFramework.Instance.Animation.RemoveEvent(m_kTextKeyInfo.Text, m_kTextKeyInfo.Time,
						MAnimation.EventType.EffectEvent);
					MFramework.Instance.Animation.ResetAnimations();
				}

				if (MFramework.Instance.Animation.AddEffectEvent(txtTextKey.Text.Trim(),
				float.Parse(txtTime.Text.Trim()), txtEffectName.Text.Trim(), cbAttachPoint.Text,
				Convert.ToSingle(nsEffectScale.Value), ckAttach.Checked, ckLoop.Checked))
				{
					DialogResult = DialogResult.OK;
				}
				else
				{
					bRet = false;
				}
			}
			else if (cbEventType.Text == "Sound Event")
			{
				if (txtTextKey.Text.Trim().Length == 0)
				{
					txtTextKey.Focus();
					MessageBox.Show("TextKey Name을 입력해주세요");
					return;
				}
				if(txtSoundName.Text.Trim().Length == 0)
				{
					txtSoundName.Focus();
					MessageBox.Show("Sound Symbol을 입력해주세요.");
					return;
				}

				// 수정해야 한다면, 기존 Event를 지우고 추가한다.
				if (m_kTextKeyInfo != null)
				{
					MFramework.Instance.Animation.RemoveEvent(m_kTextKeyInfo.Text, m_kTextKeyInfo.Time,
						MAnimation.EventType.SoundEvent);
					MFramework.Instance.Animation.ResetAnimations();
				}

				if (MFramework.Instance.Animation.AddSoundEvent(txtTextKey.Text,
				float.Parse(txtTime.Text), txtSoundName.Text, Convert.ToSingle(nsVolume.Value),
				Convert.ToSingle(nsMinDistance.Value), Convert.ToSingle(nsMaxDistance.Value)))
				{
					DialogResult = DialogResult.OK;
				}
				else
				{
					bRet = false;
				}
			}

			if (!bRet)
			{
				MessageBox.Show("Event를 추가할 수 없습니다");
			}
		}

		private void btnBrowseEffect_Click(object sender, EventArgs e)
		{
			string kSelectedItem = null;
			if (EventList.LookUpEventList("Effect"))
			{
				if (SelectEvent("Effect", ref kSelectedItem))
				{
					txtEffectName.Text = kSelectedItem;
				}
			}
		}

		private void btnBrowseSound_Click(object sender, EventArgs e)
		{
			string kSelectedItem = null;
			if (EventList.LookUpEventList("Sound"))
			{
				if (SelectEvent("Sound", ref kSelectedItem))
				{
					txtSoundName.Text = kSelectedItem;
				}
			}
		}
		
		public bool SelectEvent(string kType, ref string kSelectedItem)
		{
			ArrayList akEventList = null;
			string EventObjectPath = null;
			if (kType == "Effect")
			{
				akEventList = MFramework.Instance.EffectMan.GetEventPathList();
				EventObjectPath = MFramework.Instance.EffectMan.GetLastEventPath();
			}
			else if (kType == "Sound")
			{
				akEventList = MFramework.Instance.AudioMan.GetEventPathList();
				EventObjectPath = MFramework.Instance.AudioMan.GetLastEventPath();
			}
			else
			{
				return false;
			}

			EventItemListForm kListForm = new EventItemListForm();
			kListForm.SetAdditionalInfo(EventObjectPath, kType);
			kListForm.UpdateEventList(ref akEventList);
			if (kListForm.ShowDialog() == System.Windows.Forms.DialogResult.OK)
			{
				kSelectedItem = kListForm.GetSelectedItem();
				return true;
			}
			return false;
		}

		private void txtTime_Leave(object sender, EventArgs e)
		{
			MSequence kSeq = MFramework.Instance.Animation.ActiveSequence;
			if (kSeq == null)
			{
				return;
			}

			float fTime;
			if (float.TryParse(txtTime.Text, out fTime))
			{
				fTime = Math.Max(fTime, 0.0f);
				fTime = Math.Min(fTime, kSeq.Length);
			}
			else
			{
				fTime = MFramework.Instance.Animation.CurrentTime;
			}

			txtTime.Text = fTime.ToString("f3");
        }

        private void OnBtnRemovedAttachPointDown(object sender, MouseEventArgs e)
        {
            if (MFramework.Instance.AttachPointList.RemoveAttachPoint(cbAttachPoint.Text) == true)
            {
                cbAttachPoint.Items.RemoveAt(cbAttachPoint.SelectedIndex);
                SetAttachPointSelectedIndex(0);
            }
        }


        private void OnBtnAddAttachPointDown(object sender, MouseEventArgs e)
        {
            AddAttachPoint(cbAttachPoint.Text);
        }
        private bool AddAttachPoint(String Text)
        {
            MFramework.Instance.AttachPointList.AddAttachPoint(Text);
            if (cbAttachPoint.FindString(Text, 0) == -1)
            {
                cbAttachPoint.Items.Add(Text);
            }
            return true;
        }
        private void OnAttachPointSelectionChanged(object sender, EventArgs e)
        {
            this.Btn_RemoveAttachPoint.Show();
            this.Btn_AddAttachPoint.Hide();
        }

        private void OnAttachPointTextChanged(object sender, EventArgs e)
        {
            this.Btn_RemoveAttachPoint.Hide();
            this.Btn_AddAttachPoint.Show();

        }

        private void OnTextKeyFormClosed(object sender, FormClosedEventArgs e)
        {
        }

        private void OnAddBtnDown(object sender, MouseEventArgs e)
        {
            MFramework.Instance.AttachPointList.Save();
        }

        private void OnCanelBtnDown(object sender, MouseEventArgs e)
        {
            MFramework.Instance.AttachPointList.Reload();
        }

		//private bool LookUpEventListPath(string kType, ref string EventListPath)
		//{
		//    string kRegKey = kType + "EventObjectPath";
		//    bool bLookUpEventList = false;
		//    if (!AppUtils.RestoreRegKey(kRegKey, ref EventListPath) ||
		//        !System.IO.File.Exists(EventListPath))
		//    {
		//        bLookUpEventList = true;
		//    }

		//    if (bLookUpEventList)
		//    {
		//        this.ofdSelectEventList.Title = "Select Event Path Xml..(" + kType + ")";
		//        if (ofdSelectEventList.ShowDialog() == DialogResult.OK)
		//        {
		//            EventListPath = ofdSelectEventList.FileName;
		//            if (System.IO.File.Exists(EventListPath))
		//            {
		//                AppUtils.StoreRegKey(kRegKey, EventListPath);
		//            }
		//        }
		//        else
		//        {
		//            return false;
		//        }
		//    }
		//    return true;
		//}

		//private bool LookUpEventList(string kType)
		//{
		//    string EventListPath = null;
		//    // Event List Xml이 있는 패스를 구한다.
		//    if (!LookUpEventListPath(kType, ref EventListPath))
		//    {
		//        return false;
		//    }

		//    if (kType == "Effect")
		//    {
		//        // Path를 구했으면, Xml파일을 파싱한다.
		//        if (!MFramework.Instance.EffectMan.AddEventItems(EventListPath, kType))
		//        {
		//            return false;
		//        }
		//    }
		//    else if (kType == "Sound")
		//    {
		//        // Path를 구했으면, Xml파일을 파싱한다.
		//        if (!MFramework.Instance.AudioMan.AddEventItems(EventListPath, kType))
		//        {
		//            return false;
		//        }
		//    }
			
		//    return true;
		//}

		//private bool SelectEvent(string kType, ref string kSelectedItem)
		//{
		//    ArrayList akEventList = null;
		//    string EventObjectPath = null;
		//    if (kType == "Effect")
		//    {
		//        akEventList = MFramework.Instance.EffectMan.GetEventPathList();
		//        EventObjectPath = MFramework.Instance.EffectMan.GetLastEventPath();
		//    }
		//    else if (kType == "Sound")
		//    {
		//        akEventList = MFramework.Instance.AudioMan.GetEventPathList();
		//        EventObjectPath = MFramework.Instance.AudioMan.GetLastEventPath();
		//    }
		//    else
		//    {
		//        return false;
		//    }
			
		//    EventItemListForm kListForm = new EventItemListForm();
		//    kListForm.SetAdditionalInfo(EventObjectPath, kType);
		//    kListForm.UpdateEventList(ref akEventList);
		//    if (kListForm.ShowDialog() == DialogResult.OK)
		//    {
		//        kSelectedItem = kListForm.GetSelectedItem();
		//        return true;
		//    }
		//    return false;			
		//}
	}
}

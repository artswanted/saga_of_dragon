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
using AnimationToolUI.Source.TextKeyType;

namespace AnimationToolUI
{
	/// <summary>
	/// Summary description for HelpForm.
	/// </summary>
	public class TextKeyForm : System.Windows.Forms.Form
	{
		private Label label1;
        private Label label3;
        public Label lbEffectName;
        public Label lbAttachPoint;
        public Label lbScale;
        private Label label7;
        public Label lbSoundName;
        public Label lbVolume;
        public Label lbMinDistance;
        public Label lbMaxDistance;
        public ComboBox cbEventType;
        public ComboBox cbAttachPoint;
        public GroupBox gbBase;
        public GroupBox gbEffect;
        public GroupBox gbSound;
        public TextBox txtEffectName;
        public TextBox txtTime;
        public TextBox txtSoundName;
        public TextBox txtTextKey;

        public Button btnAdd;
        public Button btnCancel;
        public Button btnBrowseEffect;
        public Button btnBrowseSound;
        public NumericSlider nsEffectScale;
        public NumericSlider nsMaxDistance;
        public NumericSlider nsMinDistance;
        public NumericSlider nsVolume;

        private TKT_Base m_kCurrentTextKeyType;
        public Button Btn_RemoveAttachPoint;
        public Button Btn_AddAttachPoint;
        public CheckBox cbUseFadeIn;
        public Label lbFadeIn;
        public TextBox tbFadeInTime;
        public GroupBox gbFadeOutEffect;
        public Label lbFadeOutEffectName;
        public Label lbFadeOutEffectTime;
        public TextBox tbFadeOutTime;
        public ComboBox cbEffectName;
        public CheckBox cbFadeOutWhenSequenceChanged;
        public Label lbFadeOutWhenSequenceChanged;
        public TextBox tbFadeOutTimeWhenSequenceChanged;

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

            InitTextKeyTypes();
		}

        private void InitTextKeyTypes()
        {
            TKT_Base kTextKeyType = new TKT_EffectEvent();
            kTextKeyType.Initialize(this);
            kTextKeyType.OnDeselected();

            kTextKeyType = new TKT_TextKeyEvent();
            kTextKeyType.Initialize(this);
            kTextKeyType.OnDeselected();

            kTextKeyType = new TKT_SoundEvent();
            kTextKeyType.Initialize(this);
            kTextKeyType.OnDeselected();

            kTextKeyType = new TKT_FadeOutEffectEvent();
            kTextKeyType.Initialize(this);
            kTextKeyType.OnDeselected();

            CreateTextKeyType("TextKey");
            m_kCurrentTextKeyType.OnSelected();
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
            this.lbEffectName = new System.Windows.Forms.Label();
            this.txtEffectName = new System.Windows.Forms.TextBox();
            this.btnAdd = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.gbEffect = new System.Windows.Forms.GroupBox();
            this.Btn_RemoveAttachPoint = new System.Windows.Forms.Button();
            this.Btn_AddAttachPoint = new System.Windows.Forms.Button();
            this.cbUseFadeIn = new System.Windows.Forms.CheckBox();
            this.btnBrowseEffect = new System.Windows.Forms.Button();
            this.lbFadeIn = new System.Windows.Forms.Label();
            this.lbScale = new System.Windows.Forms.Label();
            this.cbAttachPoint = new System.Windows.Forms.ComboBox();
            this.lbAttachPoint = new System.Windows.Forms.Label();
            this.tbFadeInTime = new System.Windows.Forms.TextBox();
            this.gbSound = new System.Windows.Forms.GroupBox();
            this.lbMaxDistance = new System.Windows.Forms.Label();
            this.lbMinDistance = new System.Windows.Forms.Label();
            this.lbVolume = new System.Windows.Forms.Label();
            this.lbSoundName = new System.Windows.Forms.Label();
            this.txtSoundName = new System.Windows.Forms.TextBox();
            this.btnBrowseSound = new System.Windows.Forms.Button();
            this.gbFadeOutEffect = new System.Windows.Forms.GroupBox();
            this.lbFadeOutEffectName = new System.Windows.Forms.Label();
            this.lbFadeOutEffectTime = new System.Windows.Forms.Label();
            this.cbEffectName = new System.Windows.Forms.ComboBox();
            this.tbFadeOutTime = new System.Windows.Forms.TextBox();
            this.nsMaxDistance = new AnimationToolUI.NumericSlider();
            this.nsMinDistance = new AnimationToolUI.NumericSlider();
            this.nsVolume = new AnimationToolUI.NumericSlider();
            this.nsEffectScale = new AnimationToolUI.NumericSlider();
            this.lbFadeOutWhenSequenceChanged = new System.Windows.Forms.Label();
            this.tbFadeOutTimeWhenSequenceChanged = new System.Windows.Forms.TextBox();
            this.cbFadeOutWhenSequenceChanged = new System.Windows.Forms.CheckBox();
            this.gbBase.SuspendLayout();
            this.gbEffect.SuspendLayout();
            this.gbSound.SuspendLayout();
            this.gbFadeOutEffect.SuspendLayout();
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
            "Sound Event",
            "FadeOutEffect Event"});
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
            // lbEffectName
            // 
            this.lbEffectName.AutoSize = true;
            this.lbEffectName.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lbEffectName.Location = new System.Drawing.Point(15, 21);
            this.lbEffectName.Name = "lbEffectName";
            this.lbEffectName.Size = new System.Drawing.Size(34, 13);
            this.lbEffectName.TabIndex = 1;
            this.lbEffectName.Text = "Name";
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
            this.btnAdd.Location = new System.Drawing.Point(63, 544);
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
            this.btnCancel.Location = new System.Drawing.Point(157, 544);
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
            this.gbEffect.Controls.Add(this.cbFadeOutWhenSequenceChanged);
            this.gbEffect.Controls.Add(this.cbUseFadeIn);
            this.gbEffect.Controls.Add(this.btnBrowseEffect);
            this.gbEffect.Controls.Add(this.lbFadeOutWhenSequenceChanged);
            this.gbEffect.Controls.Add(this.lbFadeIn);
            this.gbEffect.Controls.Add(this.lbScale);
            this.gbEffect.Controls.Add(this.cbAttachPoint);
            this.gbEffect.Controls.Add(this.lbAttachPoint);
            this.gbEffect.Controls.Add(this.lbEffectName);
            this.gbEffect.Controls.Add(this.nsEffectScale);
            this.gbEffect.Controls.Add(this.tbFadeOutTimeWhenSequenceChanged);
            this.gbEffect.Controls.Add(this.tbFadeInTime);
            this.gbEffect.Controls.Add(this.txtEffectName);
            this.gbEffect.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.gbEffect.Location = new System.Drawing.Point(13, 123);
            this.gbEffect.Name = "gbEffect";
            this.gbEffect.Size = new System.Drawing.Size(270, 182);
            this.gbEffect.TabIndex = 2;
            this.gbEffect.TabStop = false;
            this.gbEffect.Text = "Effect";
            this.gbEffect.EnabledChanged += new System.EventHandler(this.gbEffect_EnabledChanged);
            // 
            // Btn_RemoveAttachPoint
            // 
            this.Btn_RemoveAttachPoint.Location = new System.Drawing.Point(250, 44);
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
            this.Btn_AddAttachPoint.Click += new System.EventHandler(this.Btn_AddAttachPoint_Click);
            this.Btn_AddAttachPoint.MouseDown += new System.Windows.Forms.MouseEventHandler(this.OnBtnAddAttachPointDown);
            // 
            // cbUseFadeIn
            // 
            this.cbUseFadeIn.AutoSize = true;
            this.cbUseFadeIn.Location = new System.Drawing.Point(220, 102);
            this.cbUseFadeIn.Name = "cbUseFadeIn";
            this.cbUseFadeIn.Size = new System.Drawing.Size(44, 17);
            this.cbUseFadeIn.TabIndex = 7;
            this.cbUseFadeIn.Text = "Use";
            this.cbUseFadeIn.UseVisualStyleBackColor = true;
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
            // lbFadeIn
            // 
            this.lbFadeIn.AutoSize = true;
            this.lbFadeIn.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lbFadeIn.Location = new System.Drawing.Point(14, 103);
            this.lbFadeIn.Name = "lbFadeIn";
            this.lbFadeIn.Size = new System.Drawing.Size(69, 13);
            this.lbFadeIn.TabIndex = 1;
            this.lbFadeIn.Text = "Fade In Time";
            // 
            // lbScale
            // 
            this.lbScale.AutoSize = true;
            this.lbScale.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lbScale.Location = new System.Drawing.Point(15, 75);
            this.lbScale.Name = "lbScale";
            this.lbScale.Size = new System.Drawing.Size(32, 13);
            this.lbScale.TabIndex = 1;
            this.lbScale.Text = "Scale";
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
            // lbAttachPoint
            // 
            this.lbAttachPoint.AutoSize = true;
            this.lbAttachPoint.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lbAttachPoint.Location = new System.Drawing.Point(15, 48);
            this.lbAttachPoint.Name = "lbAttachPoint";
            this.lbAttachPoint.Size = new System.Drawing.Size(66, 13);
            this.lbAttachPoint.TabIndex = 1;
            this.lbAttachPoint.Text = "Attach Point";
            // 
            // tbFadeInTime
            // 
            this.tbFadeInTime.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tbFadeInTime.Location = new System.Drawing.Point(92, 100);
            this.tbFadeInTime.Name = "tbFadeInTime";
            this.tbFadeInTime.Size = new System.Drawing.Size(127, 21);
            this.tbFadeInTime.TabIndex = 2;
            // 
            // gbSound
            // 
            this.gbSound.Controls.Add(this.lbMaxDistance);
            this.gbSound.Controls.Add(this.lbMinDistance);
            this.gbSound.Controls.Add(this.lbVolume);
            this.gbSound.Controls.Add(this.lbSoundName);
            this.gbSound.Controls.Add(this.txtSoundName);
            this.gbSound.Controls.Add(this.btnBrowseSound);
            this.gbSound.Controls.Add(this.nsMaxDistance);
            this.gbSound.Controls.Add(this.nsMinDistance);
            this.gbSound.Controls.Add(this.nsVolume);
            this.gbSound.Location = new System.Drawing.Point(12, 311);
            this.gbSound.Name = "gbSound";
            this.gbSound.Size = new System.Drawing.Size(271, 145);
            this.gbSound.TabIndex = 2;
            this.gbSound.TabStop = false;
            this.gbSound.Text = "Sound";
            this.gbSound.EnabledChanged += new System.EventHandler(this.gbSound_EnabledChanged);
            // 
            // lbMaxDistance
            // 
            this.lbMaxDistance.AutoSize = true;
            this.lbMaxDistance.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lbMaxDistance.Location = new System.Drawing.Point(15, 110);
            this.lbMaxDistance.Name = "lbMaxDistance";
            this.lbMaxDistance.Size = new System.Drawing.Size(71, 13);
            this.lbMaxDistance.TabIndex = 1;
            this.lbMaxDistance.Text = "Max Distance";
            // 
            // lbMinDistance
            // 
            this.lbMinDistance.AutoSize = true;
            this.lbMinDistance.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lbMinDistance.Location = new System.Drawing.Point(15, 80);
            this.lbMinDistance.Name = "lbMinDistance";
            this.lbMinDistance.Size = new System.Drawing.Size(67, 13);
            this.lbMinDistance.TabIndex = 1;
            this.lbMinDistance.Text = "Min Distance";
            // 
            // lbVolume
            // 
            this.lbVolume.AutoSize = true;
            this.lbVolume.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lbVolume.Location = new System.Drawing.Point(15, 47);
            this.lbVolume.Name = "lbVolume";
            this.lbVolume.Size = new System.Drawing.Size(41, 13);
            this.lbVolume.TabIndex = 1;
            this.lbVolume.Text = "Volume";
            this.lbVolume.Click += new System.EventHandler(this.label9_Click);
            // 
            // lbSoundName
            // 
            this.lbSoundName.AutoSize = true;
            this.lbSoundName.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lbSoundName.Location = new System.Drawing.Point(15, 21);
            this.lbSoundName.Name = "lbSoundName";
            this.lbSoundName.Size = new System.Drawing.Size(34, 13);
            this.lbSoundName.TabIndex = 1;
            this.lbSoundName.Text = "Name";
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
            // gbFadeOutEffect
            // 
            this.gbFadeOutEffect.Controls.Add(this.tbFadeOutTime);
            this.gbFadeOutEffect.Controls.Add(this.cbEffectName);
            this.gbFadeOutEffect.Controls.Add(this.lbFadeOutEffectName);
            this.gbFadeOutEffect.Controls.Add(this.lbFadeOutEffectTime);
            this.gbFadeOutEffect.Location = new System.Drawing.Point(12, 462);
            this.gbFadeOutEffect.Name = "gbFadeOutEffect";
            this.gbFadeOutEffect.Size = new System.Drawing.Size(271, 76);
            this.gbFadeOutEffect.TabIndex = 7;
            this.gbFadeOutEffect.TabStop = false;
            this.gbFadeOutEffect.Text = "FadeOutEffect";
            // 
            // lbFadeOutEffectName
            // 
            this.lbFadeOutEffectName.AutoSize = true;
            this.lbFadeOutEffectName.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lbFadeOutEffectName.Location = new System.Drawing.Point(15, 21);
            this.lbFadeOutEffectName.Name = "lbFadeOutEffectName";
            this.lbFadeOutEffectName.Size = new System.Drawing.Size(66, 13);
            this.lbFadeOutEffectName.TabIndex = 1;
            this.lbFadeOutEffectName.Text = "Effect Name";
            // 
            // lbFadeOutEffectTime
            // 
            this.lbFadeOutEffectTime.AutoSize = true;
            this.lbFadeOutEffectTime.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lbFadeOutEffectTime.Location = new System.Drawing.Point(15, 47);
            this.lbFadeOutEffectTime.Name = "lbFadeOutEffectTime";
            this.lbFadeOutEffectTime.Size = new System.Drawing.Size(77, 13);
            this.lbFadeOutEffectTime.TabIndex = 1;
            this.lbFadeOutEffectTime.Text = "Fade Out Time";
            // 
            // cbEffectName
            // 
            this.cbEffectName.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbEffectName.FormattingEnabled = true;
            this.cbEffectName.Location = new System.Drawing.Point(93, 13);
            this.cbEffectName.Name = "cbEffectName";
            this.cbEffectName.Size = new System.Drawing.Size(158, 21);
            this.cbEffectName.TabIndex = 3;
            // 
            // tbFadeOutTime
            // 
            this.tbFadeOutTime.Location = new System.Drawing.Point(93, 44);
            this.tbFadeOutTime.Name = "tbFadeOutTime";
            this.tbFadeOutTime.Size = new System.Drawing.Size(158, 21);
            this.tbFadeOutTime.TabIndex = 4;
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
            this.nsMinDistance.Load += new System.EventHandler(this.nsMinDistance_Load);
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
            // lbFadeOutWhenSequenceChanged
            // 
            this.lbFadeOutWhenSequenceChanged.AutoSize = true;
            this.lbFadeOutWhenSequenceChanged.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lbFadeOutWhenSequenceChanged.Location = new System.Drawing.Point(14, 131);
            this.lbFadeOutWhenSequenceChanged.Name = "lbFadeOutWhenSequenceChanged";
            this.lbFadeOutWhenSequenceChanged.Size = new System.Drawing.Size(173, 13);
            this.lbFadeOutWhenSequenceChanged.TabIndex = 1;
            this.lbFadeOutWhenSequenceChanged.Text = "Fade Out When Sequece Changed";
            // 
            // tbFadeOutTimeWhenSequenceChanged
            // 
            this.tbFadeOutTimeWhenSequenceChanged.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tbFadeOutTimeWhenSequenceChanged.Location = new System.Drawing.Point(18, 147);
            this.tbFadeOutTimeWhenSequenceChanged.Name = "tbFadeOutTimeWhenSequenceChanged";
            this.tbFadeOutTimeWhenSequenceChanged.Size = new System.Drawing.Size(127, 21);
            this.tbFadeOutTimeWhenSequenceChanged.TabIndex = 2;
            // 
            // cbFadeOutWhenSequenceChanged
            // 
            this.cbFadeOutWhenSequenceChanged.AutoSize = true;
            this.cbFadeOutWhenSequenceChanged.Location = new System.Drawing.Point(151, 149);
            this.cbFadeOutWhenSequenceChanged.Name = "cbFadeOutWhenSequenceChanged";
            this.cbFadeOutWhenSequenceChanged.Size = new System.Drawing.Size(44, 17);
            this.cbFadeOutWhenSequenceChanged.TabIndex = 7;
            this.cbFadeOutWhenSequenceChanged.Text = "Use";
            this.cbFadeOutWhenSequenceChanged.UseVisualStyleBackColor = true;
            // 
            // TextKeyForm
            // 
            this.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.ClientSize = new System.Drawing.Size(299, 575);
            this.Controls.Add(this.gbFadeOutEffect);
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
            this.gbFadeOutEffect.ResumeLayout(false);
            this.gbFadeOutEffect.PerformLayout();
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

            if (m_kCurrentTextKeyType.GetInfo() == null)
			{
				cbEventType.SelectedIndex = 0;
                SetAttachPointSelectedIndex(0);
			}

		}

        private void CreateTextKeyType(MSequence.MTextKey kTextKey)
        {
            String kTypeName = "Unknown";
            if (kTextKey.ExtraData is MSequence.MTextKey.MEffectData)
            {
                kTypeName = "EffectKey";
            }
            else if (kTextKey.ExtraData is MSequence.MTextKey.MAudioData)
            {
                kTypeName = "SoundKey";
            }
            else if (kTextKey.ExtraData is MSequence.MTextKey.MFadeOutEffectData)
            {
                kTypeName = "FadeOutEffectKey";
            }
            else
            {
                kTypeName = "TextKey";
            }

            CreateTextKeyType(kTypeName);
        }
        private void CreateTextKeyType(String kType)
        {
            if (m_kCurrentTextKeyType != null &&
                m_kCurrentTextKeyType.GetTextKeyType() == kType)
            {
                return;
            }

            if (kType == "EffectKey")
            {
                m_kCurrentTextKeyType = new TKT_EffectEvent();
            }
            else if (kType == "SoundKey")
            {
                m_kCurrentTextKeyType = new TKT_SoundEvent();
            }
            else if (kType == "TextKey")
            {
                m_kCurrentTextKeyType = new TKT_TextKeyEvent();
            }
            else if (kType == "FadeOutEffectKey")
            {
                m_kCurrentTextKeyType = new TKT_FadeOutEffectEvent();
            }
            m_kCurrentTextKeyType.Initialize(this);
        }
 
		public void SetEditTextKeyInfo(MSequence.MTextKey kTextKey)
		{
            CreateTextKeyType(kTextKey);

            m_kCurrentTextKeyType.SetInfo(kTextKey);
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
            if (m_kCurrentTextKeyType != null)
            {
                m_kCurrentTextKeyType.OnDeselected();
            }

			if (cbEventType.Text == "TextKey Event")
			{
                CreateTextKeyType("TextKey");
			}
			else if (cbEventType.Text == "Effect Event")
			{
                CreateTextKeyType("EffectKey");
            }
            else if (cbEventType.Text == "Sound Event")
            {
                CreateTextKeyType("SoundKey");
            }
            else if (cbEventType.Text == "FadeOutEffect Event")
            {
                CreateTextKeyType("FadeOutEffectKey");
            }

            m_kCurrentTextKeyType.OnSelected();
		}

		private void btnAdd_Click(object sender, EventArgs e)
		{
            MFramework.Instance.AttachPointList.Save();

            if (m_kCurrentTextKeyType.AddToAnimation() == false)
            {
                MessageBox.Show("Event를 추가할 수 없습니다");
                return;
            }

            DialogResult = DialogResult.OK;
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
            
        }

        private void OnCanelBtnDown(object sender, MouseEventArgs e)
        {
            MFramework.Instance.AttachPointList.Reload();
        }

        private void Btn_AddAttachPoint_Click(object sender, EventArgs e)
        {

        }

        private void label9_Click(object sender, EventArgs e)
        {

        }

        private void nsMinDistance_Load(object sender, EventArgs e)
        {

        }

	}
}

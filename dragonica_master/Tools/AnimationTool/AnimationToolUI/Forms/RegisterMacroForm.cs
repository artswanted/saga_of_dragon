using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using NiManagedToolInterface;

namespace AnimationToolUI
{
    class RegisterMacroForm : System.Windows.Forms.Form
	{
		private Button btnRegister;
		private TextBox txtMacroName;
		private Label label1;
		private Label label2;
		private TextBox txtEventType;
		private Label label3;
		private TextBox txtEventName;
		private Label label4;
		private TextBox txtEventTime;
		private Button btnCancel;
		private GroupBox groupBox1;
        /// <summary>
        /// Required designer variable.
        /// </summary>
		private System.ComponentModel.IContainer components = null;
		private TextBox txtKey;
		private Label label5;
		private MSequence.MTextKey m_kTextKeyInfo = null;
		private Label label6;
		private TextBox txtModifier;
		private string m_strCapturedKey;
		private Keys m_kKeyData;
		private bool m_bFocusOnKeyForm = false;

        public RegisterMacroForm()
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
			this.btnRegister = new System.Windows.Forms.Button();
			this.txtMacroName = new System.Windows.Forms.TextBox();
			this.label1 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.txtEventType = new System.Windows.Forms.TextBox();
			this.label3 = new System.Windows.Forms.Label();
			this.txtEventName = new System.Windows.Forms.TextBox();
			this.label4 = new System.Windows.Forms.Label();
			this.txtEventTime = new System.Windows.Forms.TextBox();
			this.btnCancel = new System.Windows.Forms.Button();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.label6 = new System.Windows.Forms.Label();
			this.txtModifier = new System.Windows.Forms.TextBox();
			this.txtKey = new System.Windows.Forms.TextBox();
			this.label5 = new System.Windows.Forms.Label();
			this.groupBox1.SuspendLayout();
			this.SuspendLayout();
			// 
			// btnRegister
			// 
			this.btnRegister.Location = new System.Drawing.Point(244, 25);
			this.btnRegister.Name = "btnRegister";
			this.btnRegister.Size = new System.Drawing.Size(88, 92);
			this.btnRegister.TabIndex = 0;
			this.btnRegister.Text = "Register !";
			this.btnRegister.UseVisualStyleBackColor = true;
			this.btnRegister.Click += new System.EventHandler(this.btnRegister_Click);
			// 
			// txtMacroName
			// 
			this.txtMacroName.Location = new System.Drawing.Point(113, 25);
			this.txtMacroName.Name = "txtMacroName";
			this.txtMacroName.Size = new System.Drawing.Size(125, 22);
			this.txtMacroName.TabIndex = 1;
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(20, 28);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(74, 14);
			this.label1.TabIndex = 2;
			this.label1.Text = "Macro Name";
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(20, 98);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(71, 14);
			this.label2.TabIndex = 2;
			this.label2.Text = "Event Type";
			// 
			// txtEventType
			// 
			this.txtEventType.Enabled = false;
			this.txtEventType.Location = new System.Drawing.Point(113, 95);
			this.txtEventType.Name = "txtEventType";
			this.txtEventType.Size = new System.Drawing.Size(125, 22);
			this.txtEventType.TabIndex = 1;
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Location = new System.Drawing.Point(20, 133);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(74, 14);
			this.label3.TabIndex = 2;
			this.label3.Text = "Event Name";
			// 
			// txtEventName
			// 
			this.txtEventName.Enabled = false;
			this.txtEventName.Location = new System.Drawing.Point(113, 130);
			this.txtEventName.Name = "txtEventName";
			this.txtEventName.Size = new System.Drawing.Size(125, 22);
			this.txtEventName.TabIndex = 1;
			// 
			// label4
			// 
			this.label4.AutoSize = true;
			this.label4.Location = new System.Drawing.Point(20, 168);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(70, 14);
			this.label4.TabIndex = 2;
			this.label4.Text = "Event Time";
			// 
			// txtEventTime
			// 
			this.txtEventTime.Enabled = false;
			this.txtEventTime.Location = new System.Drawing.Point(113, 165);
			this.txtEventTime.Name = "txtEventTime";
			this.txtEventTime.Size = new System.Drawing.Size(125, 22);
			this.txtEventTime.TabIndex = 1;
			// 
			// btnCancel
			// 
			this.btnCancel.Location = new System.Drawing.Point(244, 130);
			this.btnCancel.Name = "btnCancel";
			this.btnCancel.Size = new System.Drawing.Size(88, 57);
			this.btnCancel.TabIndex = 0;
			this.btnCancel.Text = "Cancel";
			this.btnCancel.UseVisualStyleBackColor = true;
			this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.label6);
			this.groupBox1.Controls.Add(this.txtModifier);
			this.groupBox1.Controls.Add(this.btnRegister);
			this.groupBox1.Controls.Add(this.label4);
			this.groupBox1.Controls.Add(this.btnCancel);
			this.groupBox1.Controls.Add(this.label3);
			this.groupBox1.Controls.Add(this.txtKey);
			this.groupBox1.Controls.Add(this.txtMacroName);
			this.groupBox1.Controls.Add(this.label2);
			this.groupBox1.Controls.Add(this.txtEventType);
			this.groupBox1.Controls.Add(this.label5);
			this.groupBox1.Controls.Add(this.label1);
			this.groupBox1.Controls.Add(this.txtEventName);
			this.groupBox1.Controls.Add(this.txtEventTime);
			this.groupBox1.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.groupBox1.Location = new System.Drawing.Point(12, 12);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(347, 204);
			this.groupBox1.TabIndex = 3;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Register Macro";
			// 
			// label6
			// 
			this.label6.AutoSize = true;
			this.label6.Location = new System.Drawing.Point(165, 63);
			this.label6.Name = "label6";
			this.label6.Size = new System.Drawing.Size(15, 14);
			this.label6.TabIndex = 4;
			this.label6.Text = "+";
			// 
			// txtModifier
			// 
			this.txtModifier.Location = new System.Drawing.Point(185, 60);
			this.txtModifier.Name = "txtModifier";
			this.txtModifier.Size = new System.Drawing.Size(53, 22);
			this.txtModifier.TabIndex = 3;
			this.txtModifier.Enter += new System.EventHandler(this.txtModifier_Enter);
			this.txtModifier.Leave += new System.EventHandler(this.txtModifier_Leave);
			// 
			// txtKey
			// 
			this.txtKey.Location = new System.Drawing.Point(113, 60);
			this.txtKey.Name = "txtKey";
			this.txtKey.Size = new System.Drawing.Size(48, 22);
			this.txtKey.TabIndex = 1;
			this.txtKey.Enter += new System.EventHandler(this.txtKey_Enter);
			this.txtKey.Leave += new System.EventHandler(this.txtKey_Leave);
			// 
			// label5
			// 
			this.label5.AutoSize = true;
			this.label5.Location = new System.Drawing.Point(20, 63);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(27, 14);
			this.label5.TabIndex = 2;
			this.label5.Text = "Key";
			// 
			// RegisterMacroForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(375, 231);
			this.Controls.Add(this.groupBox1);
			this.Font = new System.Drawing.Font("Gulim", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.Margin = new System.Windows.Forms.Padding(5, 3, 5, 3);
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "RegisterMacroForm";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "RegisterMacroForm";
			this.Load += new System.EventHandler(this.RegisterMacroForm_Load);
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			this.ResumeLayout(false);

        }

        #endregion

		private void RegisterMacroForm_Load(object sender, EventArgs e)
		{
			InitializeRegisterMacroForm();
		}

		private void InitializeRegisterMacroForm()
		{

		}

		private void btnRegister_Click(object sender, EventArgs e)
		{
			if (!MacroManager.Instance.IsAlreadyBind(m_kKeyData))
			{
				this.DialogResult = DialogResult.OK;
			}
			else
			{
				MessageBox.Show("이미 등록되어 있는 키입니다. 다른 키를 이용해주세요.");
				txtKey.Focus();
			}
		}

		private void btnCancel_Click(object sender, EventArgs e)
		{
			this.DialogResult = DialogResult.Cancel;
		}

		public void SetEventInfo(ref MSequence.MTextKey kSeq)
		{
			m_kTextKeyInfo = kSeq;
			txtEventTime.Text = kSeq.Time.ToString("f3");
			txtEventType.Text = "TEXT EVENT";

			if (kSeq.ExtraData is MSequence.MTextKey.MEffectData)
			{
				txtEventType.Text = "EFFECT EVENT";
			}
            else if (kSeq.ExtraData is MSequence.MTextKey.MAudioData)
            {
                txtEventType.Text = "SOUND EVENT";
            }
            else if (kSeq.ExtraData is MSequence.MTextKey.MFadeOutEffectData)
            {
                txtEventType.Text = "FADEOUTEFFECT EVENT";
            }
        }

		public string MacroName
		{
			get { return txtMacroName.Text; }
		}

		public Keys MacroKey
		{
			get 
			{
				return m_kKeyData; 
			}
		}

		protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
		{
			if (m_bFocusOnKeyForm)
			{
				m_strCapturedKey = keyData.ToString();

				if (m_strCapturedKey == "Escape" ||
					m_strCapturedKey == "Enter")
				{
					return base.ProcessCmdKey(ref msg, keyData);
				}

				if (m_strCapturedKey != "Tab")
				{
					// Modify our AppData... and change akKeyStrokeMap
					string strKeyBinding = null;
					string strKeyModifier = null;
					ParseCapturedKey(m_strCapturedKey,
						ref strKeyBinding, ref strKeyModifier);

					if (strKeyBinding != null)
					{
						txtKey.Text = strKeyBinding;
						txtModifier.Text = strKeyModifier;
						m_kKeyData = keyData;
					}
					else
					{
						return true;
					}

				}

				return true;
			}

			return base.ProcessCmdKey(ref msg, keyData);
		}

		private void ParseCapturedKey(string strCapturedKey,
			ref string strKeyBinding, ref string strKeyModifier)
		{
			string[] split = strCapturedKey.Split(new char[] { ',' });

			int iCount = 0;
			foreach (string str in split)
			{
				//str.Trim();
				string strTrim = str.Trim();
				split[iCount] = strTrim;

				if (strTrim == "ShiftKey" || strTrim == "ControlKey" ||
					strTrim == "AltKey" || strTrim == "Menu")
				{
					// Has no effect.
					split[iCount] = null;
				}
				iCount++;
			}

			iCount = 0;
			foreach (string str in split)
			{
				if (str == null)
					continue;

				if (str == "Shift" || str == "Control" ||
					str == "Alt")
				{
					if (strKeyModifier == null)
						strKeyModifier = str;
					else
						strKeyModifier += " + " + str;
				}
				else
				{
					if (strKeyBinding == null)
						strKeyBinding = str;
					else
						strKeyBinding += " + " + str;
				}
				iCount++;
			}
		}

		private void txtKey_Enter(object sender, EventArgs e)
		{
			m_bFocusOnKeyForm = true;
		}

		private void txtKey_Leave(object sender, EventArgs e)
		{
			m_bFocusOnKeyForm = false;
		}

		private void txtModifier_Enter(object sender, EventArgs e)
		{
			m_bFocusOnKeyForm = true;
		}

		private void txtModifier_Leave(object sender, EventArgs e)
		{
			m_bFocusOnKeyForm = false;
		}
    }
}
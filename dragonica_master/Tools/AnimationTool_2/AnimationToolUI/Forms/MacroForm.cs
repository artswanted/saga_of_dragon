using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using NiManagedToolInterface;

namespace AnimationToolUI
{
    class MacroForm : System.Windows.Forms.Form
    {
		private ListView lvMacroList;
		private Label label1;
		private Button btnRemoveEvent;
		private ColumnHeader chKey;
		private ColumnHeader chMacroName;
		private ColumnHeader chVolume;
		private ColumnHeader chMinDist;
		private ColumnHeader chMaxDist;
		private ColumnHeader chEventType;
		private ColumnHeader chEventName;
		private ColumnHeader chAttachPoint;
		private ColumnHeader chScale;
		private Button btnOK;
		private ColumnHeader chTime;
        /// <summary>
        /// Required designer variable.
        /// </summary>
		private System.ComponentModel.IContainer components = null;

        public MacroForm()
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
			this.lvMacroList = new System.Windows.Forms.ListView();
			this.chKey = new System.Windows.Forms.ColumnHeader();
			this.chMacroName = new System.Windows.Forms.ColumnHeader();
			this.chEventType = new System.Windows.Forms.ColumnHeader();
			this.chEventName = new System.Windows.Forms.ColumnHeader();
			this.chTime = new System.Windows.Forms.ColumnHeader();
			this.chVolume = new System.Windows.Forms.ColumnHeader();
			this.chMinDist = new System.Windows.Forms.ColumnHeader();
			this.chMaxDist = new System.Windows.Forms.ColumnHeader();
			this.chAttachPoint = new System.Windows.Forms.ColumnHeader();
			this.chScale = new System.Windows.Forms.ColumnHeader();
			this.label1 = new System.Windows.Forms.Label();
			this.btnRemoveEvent = new System.Windows.Forms.Button();
			this.btnOK = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// lvMacroList
			// 
			this.lvMacroList.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.chKey,
            this.chMacroName,
            this.chEventType,
            this.chEventName,
            this.chTime,
            this.chVolume,
            this.chMinDist,
            this.chMaxDist,
            this.chAttachPoint,
            this.chScale});
			this.lvMacroList.FullRowSelect = true;
			this.lvMacroList.GridLines = true;
			this.lvMacroList.Location = new System.Drawing.Point(10, 34);
			this.lvMacroList.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
			this.lvMacroList.MultiSelect = false;
			this.lvMacroList.Name = "lvMacroList";
			this.lvMacroList.Size = new System.Drawing.Size(745, 344);
			this.lvMacroList.TabIndex = 0;
			this.lvMacroList.UseCompatibleStateImageBehavior = false;
			this.lvMacroList.View = System.Windows.Forms.View.Details;
			// 
			// chKey
			// 
			this.chKey.Text = "Key";
			// 
			// chMacroName
			// 
			this.chMacroName.Text = "Macro Name";
			this.chMacroName.Width = 93;
			// 
			// chEventType
			// 
			this.chEventType.Text = "Event Type";
			this.chEventType.Width = 83;
			// 
			// chEventName
			// 
			this.chEventName.Text = "Event Name";
			this.chEventName.Width = 89;
			// 
			// chTime
			// 
			this.chTime.Text = "Time";
			// 
			// chVolume
			// 
			this.chVolume.Text = "Volume";
			this.chVolume.Width = 58;
			// 
			// chMinDist
			// 
			this.chMinDist.Text = "Min Dist";
			this.chMinDist.Width = 63;
			// 
			// chMaxDist
			// 
			this.chMaxDist.Text = "Max Dist";
			this.chMaxDist.Width = 67;
			// 
			// chAttachPoint
			// 
			this.chAttachPoint.Text = "Attach Point";
			this.chAttachPoint.Width = 85;
			// 
			// chScale
			// 
			this.chScale.Text = "Scale";
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(11, 15);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(61, 14);
			this.label1.TabIndex = 1;
			this.label1.Text = "Macro List";
			// 
			// btnRemoveEvent
			// 
			this.btnRemoveEvent.Location = new System.Drawing.Point(388, 397);
			this.btnRemoveEvent.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
			this.btnRemoveEvent.Name = "btnRemoveEvent";
			this.btnRemoveEvent.Size = new System.Drawing.Size(88, 30);
			this.btnRemoveEvent.TabIndex = 3;
			this.btnRemoveEvent.Text = "Remove";
			this.btnRemoveEvent.UseVisualStyleBackColor = true;
			this.btnRemoveEvent.Click += new System.EventHandler(this.btnRemoveEvent_Click);
			// 
			// btnOK
			// 
			this.btnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.btnOK.Location = new System.Drawing.Point(291, 397);
			this.btnOK.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
			this.btnOK.Name = "btnOK";
			this.btnOK.Size = new System.Drawing.Size(88, 30);
			this.btnOK.TabIndex = 2;
			this.btnOK.Text = "OK";
			this.btnOK.UseVisualStyleBackColor = true;
			// 
			// MacroForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 14F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(767, 444);
			this.Controls.Add(this.btnRemoveEvent);
			this.Controls.Add(this.btnOK);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.lvMacroList);
			this.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.Margin = new System.Windows.Forms.Padding(4);
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "MacroForm";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "MacroForm";
			this.Load += new System.EventHandler(this.MacroForm_Load);
			this.ResumeLayout(false);
			this.PerformLayout();

        }

        #endregion

		private void MacroForm_Load(object sender, EventArgs e)
		{
			InitializeMacroForm();
		}

		private void InitializeMacroForm()
		{

		}

		private void btnRemoveEvent_Click(object sender, EventArgs e)
		{

		}

		public void UpdateListView(Hashtable kMacroTable)
		{
			lvMacroList.Items.Clear();
			foreach (DictionaryEntry e in kMacroTable)
			{
				MacroData kData = (MacroData)e.Value;
			
				ListViewItem kItem = new ListViewItem(new string[] {
					kData.Key.ToString(),
					kData.MacroName,
					"TEXT",
					kData.EventInfo.Text,
					kData.EventInfo.Time.ToString("f3")}, 
					-1);

				if (kData.EventInfo.ExtraData is MSequence.MTextKey.MAudioData)
				{
					MSequence.MTextKey.MAudioData kAudioData = 
						(MSequence.MTextKey.MAudioData) kData.EventInfo.ExtraData;
					kItem.SubItems[2] = new ListViewItem.ListViewSubItem(kItem, "SOUND");
					kItem.SubItems.AddRange(new string[] {
						kAudioData.Volume.ToString("f1"),
						kAudioData.MinDistance.ToString("f1"),
						kAudioData.MaxDistance.ToString("f1"), "", ""});
				}
				else if (kData.EventInfo.ExtraData is MSequence.MTextKey.MEffectData)
				{
					MSequence.MTextKey.MEffectData kEffectData =
						(MSequence.MTextKey.MEffectData)kData.EventInfo.ExtraData;
					kItem.SubItems[2] = new ListViewItem.ListViewSubItem(kItem, "EFFECT");
					kItem.SubItems.AddRange(new string[] {
						"", "", "",
						kEffectData.AttachPoint,
						kEffectData.Scale.ToString("f2")});
				}
				
				lvMacroList.Items.Add(kItem);
			}
		}
    }
}
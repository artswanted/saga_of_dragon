namespace Emergent.Gamebryo.SceneDesigner.GUI.ProjectG
{
    partial class DecalPanel
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
            this.DecalCreate = new System.Windows.Forms.Button();
            this.TargeterCreate = new System.Windows.Forms.Button();
            this.m_tbWidth = new System.Windows.Forms.TextBox();
            this.m_tbHeight = new System.Windows.Forms.TextBox();
            this.m_tbDepth = new System.Windows.Forms.TextBox();
            this.LoadDecal = new System.Windows.Forms.Button();
            this.SaveDecal = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // DecalCreate
            // 
            this.DecalCreate.Location = new System.Drawing.Point(12, 50);
            this.DecalCreate.Name = "DecalCreate";
            this.DecalCreate.Size = new System.Drawing.Size(117, 30);
            this.DecalCreate.TabIndex = 0;
            this.DecalCreate.Text = "DecalCreate";
            this.DecalCreate.UseVisualStyleBackColor = true;
            this.DecalCreate.Click += new System.EventHandler(this.DecalCreate_Click);
            // 
            // TargeterCreate
            // 
            this.TargeterCreate.Location = new System.Drawing.Point(12, 12);
            this.TargeterCreate.Name = "TargeterCreate";
            this.TargeterCreate.Size = new System.Drawing.Size(117, 32);
            this.TargeterCreate.TabIndex = 1;
            this.TargeterCreate.Text = "TargeterCreate";
            this.TargeterCreate.UseVisualStyleBackColor = true;
            this.TargeterCreate.Click += new System.EventHandler(this.TargeterCreate_Click);
            // 
            // m_tbWidth
            // 
            this.m_tbWidth.Location = new System.Drawing.Point(114, 170);
            this.m_tbWidth.Name = "m_tbWidth";
            this.m_tbWidth.Size = new System.Drawing.Size(100, 21);
            this.m_tbWidth.TabIndex = 2;
            // 
            // m_tbHeight
            // 
            this.m_tbHeight.Location = new System.Drawing.Point(114, 197);
            this.m_tbHeight.Name = "m_tbHeight";
            this.m_tbHeight.Size = new System.Drawing.Size(100, 21);
            this.m_tbHeight.TabIndex = 3;
            // 
            // m_tbDepth
            // 
            this.m_tbDepth.Location = new System.Drawing.Point(114, 224);
            this.m_tbDepth.Name = "m_tbDepth";
            this.m_tbDepth.Size = new System.Drawing.Size(100, 21);
            this.m_tbDepth.TabIndex = 4;
            // 
            // LoadDecal
            // 
            this.LoadDecal.Location = new System.Drawing.Point(12, 86);
            this.LoadDecal.Name = "LoadDecal";
            this.LoadDecal.Size = new System.Drawing.Size(117, 28);
            this.LoadDecal.TabIndex = 5;
            this.LoadDecal.Text = "LoadDecal";
            this.LoadDecal.UseVisualStyleBackColor = true;
            this.LoadDecal.Click += new System.EventHandler(this.LoadDecal_Click);
            // 
            // SaveDecal
            // 
            this.SaveDecal.Location = new System.Drawing.Point(13, 121);
            this.SaveDecal.Name = "SaveDecal";
            this.SaveDecal.Size = new System.Drawing.Size(116, 29);
            this.SaveDecal.TabIndex = 6;
            this.SaveDecal.Text = "SaveDecal";
            this.SaveDecal.UseVisualStyleBackColor = true;
            this.SaveDecal.Click += new System.EventHandler(this.SaveDecal_Click);
            // 
            // DecalPanel
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(307, 494);
            this.Controls.Add(this.SaveDecal);
            this.Controls.Add(this.LoadDecal);
            this.Controls.Add(this.m_tbDepth);
            this.Controls.Add(this.m_tbHeight);
            this.Controls.Add(this.m_tbWidth);
            this.Controls.Add(this.TargeterCreate);
            this.Controls.Add(this.DecalCreate);
            this.Name = "DecalPanel";
            this.Text = "DecalPanel";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button DecalCreate;
        private System.Windows.Forms.Button TargeterCreate;
        private System.Windows.Forms.TextBox m_tbWidth;
        private System.Windows.Forms.TextBox m_tbHeight;
        private System.Windows.Forms.TextBox m_tbDepth;
        private System.Windows.Forms.Button LoadDecal;
        private System.Windows.Forms.Button SaveDecal;
    }
}
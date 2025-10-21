namespace Emergent.Gamebryo.SceneDesigner.GUI.ProjectG
{
    partial class ElevatorPanel
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
			this.lbElevator = new System.Windows.Forms.ListBox();
			this.tbID = new System.Windows.Forms.TextBox();
			this.tbSpeed = new System.Windows.Forms.TextBox();
			this.label2 = new System.Windows.Forms.Label();
			this.label3 = new System.Windows.Forms.Label();
			this.tbPosX = new System.Windows.Forms.TextBox();
			this.tbPosY = new System.Windows.Forms.TextBox();
			this.tbPosZ = new System.Windows.Forms.TextBox();
			this.tbTime = new System.Windows.Forms.TextBox();
			this.label4 = new System.Windows.Forms.Label();
			this.label5 = new System.Windows.Forms.Label();
			this.label6 = new System.Windows.Forms.Label();
			this.label7 = new System.Windows.Forms.Label();
			this.label8 = new System.Windows.Forms.Label();
			this.tbMapNum = new System.Windows.Forms.TextBox();
			this.btnLoad = new System.Windows.Forms.Button();
			this.btnSave = new System.Windows.Forms.Button();
			this.lbPoints = new System.Windows.Forms.ListBox();
			this.tbPointNum = new System.Windows.Forms.TextBox();
			this.label10 = new System.Windows.Forms.Label();
			this.btnAdd = new System.Windows.Forms.Button();
			this.btnCreate = new System.Windows.Forms.Button();
			this.btnElevatorDelete = new System.Windows.Forms.Button();
			this.btnPointRemove = new System.Windows.Forms.Button();
			this.btnModify = new System.Windows.Forms.Button();
			this.btnElevatorModify = new System.Windows.Forms.Button();
			this.label11 = new System.Windows.Forms.Label();
			this.cbRidable = new System.Windows.Forms.ComboBox();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.groupBox2 = new System.Windows.Forms.GroupBox();
			this.groupBox1.SuspendLayout();
			this.groupBox2.SuspendLayout();
			this.SuspendLayout();
			// 
			// lbElevator
			// 
			this.lbElevator.FormattingEnabled = true;
			this.lbElevator.HorizontalScrollbar = true;
			this.lbElevator.ItemHeight = 12;
			this.lbElevator.Location = new System.Drawing.Point(14, 16);
			this.lbElevator.Name = "lbElevator";
			this.lbElevator.Size = new System.Drawing.Size(252, 88);
			this.lbElevator.TabIndex = 1;
			this.lbElevator.SelectedIndexChanged += new System.EventHandler(this.lbElevator_SelectedIndexChanged);
			// 
			// tbID
			// 
			this.tbID.Location = new System.Drawing.Point(145, 110);
			this.tbID.Name = "tbID";
			this.tbID.Size = new System.Drawing.Size(121, 21);
			this.tbID.TabIndex = 2;
			// 
			// tbSpeed
			// 
			this.tbSpeed.Location = new System.Drawing.Point(145, 136);
			this.tbSpeed.Name = "tbSpeed";
			this.tbSpeed.Size = new System.Drawing.Size(121, 21);
			this.tbSpeed.TabIndex = 3;
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(74, 114);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(60, 12);
			this.label2.TabIndex = 0;
			this.label2.Text = "Class_No";
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Location = new System.Drawing.Point(93, 139);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(41, 12);
			this.label3.TabIndex = 0;
			this.label3.Text = "Speed";
			// 
			// tbPosX
			// 
			this.tbPosX.Location = new System.Drawing.Point(415, 30);
			this.tbPosX.Name = "tbPosX";
			this.tbPosX.Size = new System.Drawing.Size(100, 21);
			this.tbPosX.TabIndex = 0;
			this.tbPosX.Visible = false;
			this.tbPosX.TextChanged += new System.EventHandler(this.tbPosX_TextChanged);
			// 
			// tbPosY
			// 
			this.tbPosY.Location = new System.Drawing.Point(415, 57);
			this.tbPosY.Name = "tbPosY";
			this.tbPosY.Size = new System.Drawing.Size(100, 21);
			this.tbPosY.TabIndex = 0;
			this.tbPosY.Visible = false;
			this.tbPosY.TextChanged += new System.EventHandler(this.tbPosY_TextChanged);
			// 
			// tbPosZ
			// 
			this.tbPosZ.Location = new System.Drawing.Point(415, 84);
			this.tbPosZ.Name = "tbPosZ";
			this.tbPosZ.Size = new System.Drawing.Size(100, 21);
			this.tbPosZ.TabIndex = 0;
			this.tbPosZ.Visible = false;
			this.tbPosZ.TextChanged += new System.EventHandler(this.tbPosZ_TextChanged);
			// 
			// tbTime
			// 
			this.tbTime.Location = new System.Drawing.Point(162, 110);
			this.tbTime.Name = "tbTime";
			this.tbTime.Size = new System.Drawing.Size(100, 21);
			this.tbTime.TabIndex = 9;
			// 
			// label4
			// 
			this.label4.AutoSize = true;
			this.label4.Location = new System.Drawing.Point(333, 30);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(39, 12);
			this.label4.TabIndex = 0;
			this.label4.Text = "Pos X";
			this.label4.Visible = false;
			// 
			// label5
			// 
			this.label5.AutoSize = true;
			this.label5.Location = new System.Drawing.Point(335, 57);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(39, 12);
			this.label5.TabIndex = 0;
			this.label5.Text = "Pos Y";
			this.label5.Visible = false;
			// 
			// label6
			// 
			this.label6.AutoSize = true;
			this.label6.Location = new System.Drawing.Point(337, 84);
			this.label6.Name = "label6";
			this.label6.Size = new System.Drawing.Size(39, 12);
			this.label6.TabIndex = 0;
			this.label6.Text = "Pos Z";
			this.label6.Visible = false;
			// 
			// label7
			// 
			this.label7.AutoSize = true;
			this.label7.Location = new System.Drawing.Point(122, 114);
			this.label7.Name = "label7";
			this.label7.Size = new System.Drawing.Size(34, 12);
			this.label7.TabIndex = 13;
			this.label7.Text = "Time";
			// 
			// label8
			// 
			this.label8.AutoSize = true;
			this.label8.Location = new System.Drawing.Point(330, 148);
			this.label8.Name = "label8";
			this.label8.Size = new System.Drawing.Size(79, 12);
			this.label8.TabIndex = 0;
			this.label8.Text = "Map Number";
			this.label8.Visible = false;
			// 
			// tbMapNum
			// 
			this.tbMapNum.Location = new System.Drawing.Point(415, 145);
			this.tbMapNum.Name = "tbMapNum";
			this.tbMapNum.Size = new System.Drawing.Size(100, 21);
			this.tbMapNum.TabIndex = 0;
			this.tbMapNum.Visible = false;
			this.tbMapNum.TextChanged += new System.EventHandler(this.tbMapNum_TextChanged);
			// 
			// btnLoad
			// 
			this.btnLoad.Location = new System.Drawing.Point(332, 173);
			this.btnLoad.Name = "btnLoad";
			this.btnLoad.Size = new System.Drawing.Size(75, 23);
			this.btnLoad.TabIndex = 0;
			this.btnLoad.Text = "Load";
			this.btnLoad.UseVisualStyleBackColor = true;
			this.btnLoad.Visible = false;
			this.btnLoad.Click += new System.EventHandler(this.btnLoad_Click);
			// 
			// btnSave
			// 
			this.btnSave.Location = new System.Drawing.Point(440, 172);
			this.btnSave.Name = "btnSave";
			this.btnSave.Size = new System.Drawing.Size(75, 23);
			this.btnSave.TabIndex = 0;
			this.btnSave.Text = "Save";
			this.btnSave.UseVisualStyleBackColor = true;
			this.btnSave.Visible = false;
			this.btnSave.Click += new System.EventHandler(this.btnSave_Click);
			// 
			// lbPoints
			// 
			this.lbPoints.FormattingEnabled = true;
			this.lbPoints.HorizontalScrollbar = true;
			this.lbPoints.ItemHeight = 12;
			this.lbPoints.Location = new System.Drawing.Point(10, 16);
			this.lbPoints.Name = "lbPoints";
			this.lbPoints.Size = new System.Drawing.Size(252, 88);
			this.lbPoints.TabIndex = 8;
			this.lbPoints.DoubleClick += new System.EventHandler(this.lbPoints_DoubleClick);
			this.lbPoints.SelectedIndexChanged += new System.EventHandler(this.lbPoints_SelectedIndexChanged);
			// 
			// tbPointNum
			// 
			this.tbPointNum.Location = new System.Drawing.Point(424, 323);
			this.tbPointNum.Name = "tbPointNum";
			this.tbPointNum.Size = new System.Drawing.Size(100, 21);
			this.tbPointNum.TabIndex = 0;
			this.tbPointNum.Visible = false;
			// 
			// label10
			// 
			this.label10.AutoSize = true;
			this.label10.Location = new System.Drawing.Point(354, 326);
			this.label10.Name = "label10";
			this.label10.Size = new System.Drawing.Size(64, 12);
			this.label10.TabIndex = 0;
			this.label10.Text = "Point Num";
			this.label10.Visible = false;
			// 
			// btnAdd
			// 
			this.btnAdd.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.btnAdd.Location = new System.Drawing.Point(10, 137);
			this.btnAdd.Name = "btnAdd";
			this.btnAdd.Size = new System.Drawing.Size(75, 23);
			this.btnAdd.TabIndex = 11;
			this.btnAdd.Text = "Add";
			this.btnAdd.UseVisualStyleBackColor = true;
			this.btnAdd.Click += new System.EventHandler(this.btnAdd_Click);
			// 
			// btnCreate
			// 
			this.btnCreate.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.btnCreate.Location = new System.Drawing.Point(14, 187);
			this.btnCreate.Name = "btnCreate";
			this.btnCreate.Size = new System.Drawing.Size(75, 23);
			this.btnCreate.TabIndex = 5;
			this.btnCreate.Text = "Create";
			this.btnCreate.UseVisualStyleBackColor = true;
			this.btnCreate.Click += new System.EventHandler(this.btnCreate_Click);
			// 
			// btnElevatorDelete
			// 
			this.btnElevatorDelete.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.btnElevatorDelete.Location = new System.Drawing.Point(190, 187);
			this.btnElevatorDelete.Name = "btnElevatorDelete";
			this.btnElevatorDelete.Size = new System.Drawing.Size(75, 23);
			this.btnElevatorDelete.TabIndex = 7;
			this.btnElevatorDelete.Text = "Delete";
			this.btnElevatorDelete.UseVisualStyleBackColor = true;
			this.btnElevatorDelete.Click += new System.EventHandler(this.btnElevatorDelete_Click);
			// 
			// btnPointRemove
			// 
			this.btnPointRemove.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.btnPointRemove.Location = new System.Drawing.Point(186, 137);
			this.btnPointRemove.Name = "btnPointRemove";
			this.btnPointRemove.Size = new System.Drawing.Size(75, 23);
			this.btnPointRemove.TabIndex = 13;
			this.btnPointRemove.Text = "Remove";
			this.btnPointRemove.UseVisualStyleBackColor = true;
			this.btnPointRemove.Click += new System.EventHandler(this.btnPointRemove_Click);
			// 
			// btnModify
			// 
			this.btnModify.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.btnModify.Location = new System.Drawing.Point(98, 137);
			this.btnModify.Name = "btnModify";
			this.btnModify.Size = new System.Drawing.Size(75, 23);
			this.btnModify.TabIndex = 12;
			this.btnModify.Text = "Modify";
			this.btnModify.UseVisualStyleBackColor = true;
			this.btnModify.Click += new System.EventHandler(this.btnModify_Click);
			// 
			// btnElevatorModify
			// 
			this.btnElevatorModify.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.btnElevatorModify.Location = new System.Drawing.Point(102, 187);
			this.btnElevatorModify.Name = "btnElevatorModify";
			this.btnElevatorModify.Size = new System.Drawing.Size(75, 23);
			this.btnElevatorModify.TabIndex = 6;
			this.btnElevatorModify.Text = "Modify";
			this.btnElevatorModify.UseVisualStyleBackColor = true;
			this.btnElevatorModify.Click += new System.EventHandler(this.btnElevatorModify_Click);
			// 
			// label11
			// 
			this.label11.AutoSize = true;
			this.label11.Location = new System.Drawing.Point(87, 164);
			this.label11.Name = "label11";
			this.label11.Size = new System.Drawing.Size(47, 12);
			this.label11.TabIndex = 0;
			this.label11.Text = "Ridable";
			// 
			// cbRidable
			// 
			this.cbRidable.FormattingEnabled = true;
			this.cbRidable.Items.AddRange(new object[] {
            "FALSE",
            "TRUE"});
			this.cbRidable.Location = new System.Drawing.Point(145, 162);
			this.cbRidable.Name = "cbRidable";
			this.cbRidable.Size = new System.Drawing.Size(121, 20);
			this.cbRidable.TabIndex = 4;
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.cbRidable);
			this.groupBox1.Controls.Add(this.label11);
			this.groupBox1.Controls.Add(this.btnElevatorModify);
			this.groupBox1.Controls.Add(this.btnElevatorDelete);
			this.groupBox1.Controls.Add(this.btnCreate);
			this.groupBox1.Controls.Add(this.label3);
			this.groupBox1.Controls.Add(this.label2);
			this.groupBox1.Controls.Add(this.tbSpeed);
			this.groupBox1.Controls.Add(this.tbID);
			this.groupBox1.Controls.Add(this.lbElevator);
			this.groupBox1.Location = new System.Drawing.Point(14, 12);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(279, 224);
			this.groupBox1.TabIndex = 14;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Elevator List";
			// 
			// groupBox2
			// 
			this.groupBox2.Controls.Add(this.btnModify);
			this.groupBox2.Controls.Add(this.btnPointRemove);
			this.groupBox2.Controls.Add(this.btnAdd);
			this.groupBox2.Controls.Add(this.lbPoints);
			this.groupBox2.Controls.Add(this.label7);
			this.groupBox2.Controls.Add(this.tbTime);
			this.groupBox2.Location = new System.Drawing.Point(14, 251);
			this.groupBox2.Name = "groupBox2";
			this.groupBox2.Size = new System.Drawing.Size(279, 170);
			this.groupBox2.TabIndex = 15;
			this.groupBox2.TabStop = false;
			this.groupBox2.Text = "Point List";
			// 
			// ElevatorPanel
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.AutoScroll = true;
			this.AutoScrollMargin = new System.Drawing.Size(10, 10);
			this.ClientSize = new System.Drawing.Size(306, 454);
			this.Controls.Add(this.groupBox2);
			this.Controls.Add(this.groupBox1);
			this.Controls.Add(this.label10);
			this.Controls.Add(this.tbPointNum);
			this.Controls.Add(this.btnSave);
			this.Controls.Add(this.btnLoad);
			this.Controls.Add(this.tbMapNum);
			this.Controls.Add(this.label8);
			this.Controls.Add(this.label6);
			this.Controls.Add(this.label5);
			this.Controls.Add(this.label4);
			this.Controls.Add(this.tbPosZ);
			this.Controls.Add(this.tbPosY);
			this.Controls.Add(this.tbPosX);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.SizableToolWindow;
			this.Name = "ElevatorPanel";
			this.Text = "ElevatorPanel";
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			this.groupBox2.ResumeLayout(false);
			this.groupBox2.PerformLayout();
			this.ResumeLayout(false);
			this.PerformLayout();

        }

        #endregion

		private System.Windows.Forms.ListBox lbElevator;
        private System.Windows.Forms.TextBox tbID;
        private System.Windows.Forms.TextBox tbSpeed;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox tbPosX;
        private System.Windows.Forms.TextBox tbPosY;
        private System.Windows.Forms.TextBox tbPosZ;
        private System.Windows.Forms.TextBox tbTime;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.TextBox tbMapNum;
        private System.Windows.Forms.Button btnLoad;
        private System.Windows.Forms.Button btnSave;
		private System.Windows.Forms.ListBox lbPoints;
        private System.Windows.Forms.TextBox tbPointNum;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Button btnAdd;
        private System.Windows.Forms.Button btnCreate;
        private System.Windows.Forms.Button btnElevatorDelete;
        private System.Windows.Forms.Button btnPointRemove;
        private System.Windows.Forms.Button btnModify;
        private System.Windows.Forms.Button btnElevatorModify;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.ComboBox cbRidable;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.GroupBox groupBox2;
    }
}
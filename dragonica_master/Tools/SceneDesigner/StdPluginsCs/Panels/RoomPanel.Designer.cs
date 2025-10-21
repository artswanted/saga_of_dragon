namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Panels
{
    partial class RoomPanel
    {
        /// <summary>
        /// 필수 디자이너 변수입니다.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 사용 중인 모든 리소스를 정리합니다.
        /// </summary>
        /// <param name="disposing">관리되는 리소스를 삭제해야 하면 true이고, 그렇지 않으면 false입니다.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form 디자이너에서 생성한 코드

        /// <summary>
        /// 디자이너 지원에 필요한 메서드입니다.
        /// 이 메서드의 내용을 코드 편집기로 수정하지 마십시오.
        /// </summary>
        private void InitializeComponent()
        {
            this.label1 = new System.Windows.Forms.Label();
            this.SelectedRoomName = new System.Windows.Forms.TextBox();
            this.RoomList = new System.Windows.Forms.ListBox();
            this.label2 = new System.Windows.Forms.Label();
            this.ObjectList = new System.Windows.Forms.ListBox();
            this.label3 = new System.Windows.Forms.Label();
            this.PortalList = new System.Windows.Forms.ListBox();
            this.label4 = new System.Windows.Forms.Label();
            this.AddObject = new System.Windows.Forms.Button();
            this.RemoveObject = new System.Windows.Forms.Button();
            this.AddPortal = new System.Windows.Forms.Button();
            this.RemovePortal = new System.Windows.Forms.Button();
            this.RefreshRoomList = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 21);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(91, 12);
            this.label1.TabIndex = 0;
            this.label1.Text = "Selected Room";
            // 
            // SelectedRoomName
            // 
            this.SelectedRoomName.Location = new System.Drawing.Point(109, 18);
            this.SelectedRoomName.Name = "SelectedRoomName";
            this.SelectedRoomName.Size = new System.Drawing.Size(367, 21);
            this.SelectedRoomName.TabIndex = 1;
            // 
            // RoomList
            // 
            this.RoomList.FormattingEnabled = true;
            this.RoomList.ItemHeight = 12;
            this.RoomList.Location = new System.Drawing.Point(109, 72);
            this.RoomList.Name = "RoomList";
            this.RoomList.Size = new System.Drawing.Size(366, 160);
            this.RoomList.TabIndex = 2;
            this.RoomList.SelectedIndexChanged += new System.EventHandler(this.RoomList_OnSelectedIndexChanged);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(41, 72);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(62, 12);
            this.label2.TabIndex = 0;
            this.label2.Text = "Room List";
            // 
            // ObjectList
            // 
            this.ObjectList.FormattingEnabled = true;
            this.ObjectList.ItemHeight = 12;
            this.ObjectList.Location = new System.Drawing.Point(110, 263);
            this.ObjectList.Name = "ObjectList";
            this.ObjectList.SelectionMode = System.Windows.Forms.SelectionMode.MultiExtended;
            this.ObjectList.Size = new System.Drawing.Size(366, 160);
            this.ObjectList.TabIndex = 2;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(41, 263);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(65, 12);
            this.label3.TabIndex = 0;
            this.label3.Text = "Object List";
            // 
            // PortalList
            // 
            this.PortalList.FormattingEnabled = true;
            this.PortalList.ItemHeight = 12;
            this.PortalList.Location = new System.Drawing.Point(109, 446);
            this.PortalList.Name = "PortalList";
            this.PortalList.SelectionMode = System.Windows.Forms.SelectionMode.MultiExtended;
            this.PortalList.Size = new System.Drawing.Size(366, 160);
            this.PortalList.TabIndex = 2;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(38, 446);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(61, 12);
            this.label4.TabIndex = 0;
            this.label4.Text = "Portal List";
            // 
            // AddObject
            // 
            this.AddObject.Location = new System.Drawing.Point(31, 309);
            this.AddObject.Name = "AddObject";
            this.AddObject.Size = new System.Drawing.Size(75, 23);
            this.AddObject.TabIndex = 3;
            this.AddObject.Text = "+";
            this.AddObject.UseVisualStyleBackColor = true;
            this.AddObject.Click += new System.EventHandler(this.ObjectListAddBtn_OnClick);
            // 
            // RemoveObject
            // 
            this.RemoveObject.Location = new System.Drawing.Point(31, 338);
            this.RemoveObject.Name = "RemoveObject";
            this.RemoveObject.Size = new System.Drawing.Size(75, 23);
            this.RemoveObject.TabIndex = 3;
            this.RemoveObject.Text = "-";
            this.RemoveObject.UseVisualStyleBackColor = true;
            this.RemoveObject.Click += new System.EventHandler(this.ObjectListBtn_RemoveBtn_OnClick);
            // 
            // AddPortal
            // 
            this.AddPortal.Location = new System.Drawing.Point(31, 503);
            this.AddPortal.Name = "AddPortal";
            this.AddPortal.Size = new System.Drawing.Size(75, 23);
            this.AddPortal.TabIndex = 3;
            this.AddPortal.Text = "+";
            this.AddPortal.UseVisualStyleBackColor = true;
            this.AddPortal.Click += new System.EventHandler(this.PortalAddBtn_OnClick);
            // 
            // RemovePortal
            // 
            this.RemovePortal.Location = new System.Drawing.Point(31, 532);
            this.RemovePortal.Name = "RemovePortal";
            this.RemovePortal.Size = new System.Drawing.Size(75, 23);
            this.RemovePortal.TabIndex = 3;
            this.RemovePortal.Text = "-";
            this.RemovePortal.UseVisualStyleBackColor = true;
            this.RemovePortal.Click += new System.EventHandler(this.PortalRemoveBtn_OnClick);
            // 
            // RefreshRoomList
            // 
            this.RefreshRoomList.Location = new System.Drawing.Point(31, 98);
            this.RefreshRoomList.Name = "RefreshRoomList";
            this.RefreshRoomList.Size = new System.Drawing.Size(75, 23);
            this.RefreshRoomList.TabIndex = 3;
            this.RefreshRoomList.Text = "Refresh";
            this.RefreshRoomList.UseVisualStyleBackColor = true;
            this.RefreshRoomList.Click += new System.EventHandler(this.RefreshBtn_Click);
            // 
            // RoomPanel
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(504, 618);
            this.Controls.Add(this.RemovePortal);
            this.Controls.Add(this.RemoveObject);
            this.Controls.Add(this.AddPortal);
            this.Controls.Add(this.RefreshRoomList);
            this.Controls.Add(this.AddObject);
            this.Controls.Add(this.PortalList);
            this.Controls.Add(this.ObjectList);
            this.Controls.Add(this.RoomList);
            this.Controls.Add(this.SelectedRoomName);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Name = "RoomPanel";
            this.Text = "RoomPanel";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox SelectedRoomName;
        private System.Windows.Forms.ListBox RoomList;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ListBox ObjectList;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.ListBox PortalList;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Button AddObject;
        private System.Windows.Forms.Button RemoveObject;
        private System.Windows.Forms.Button AddPortal;
        private System.Windows.Forms.Button RemovePortal;
        private System.Windows.Forms.Button RefreshRoomList;
    }
}
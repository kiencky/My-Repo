namespace SmartCarPark
{
    partial class Doanhthu
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
            this.dateTimePicker_from = new System.Windows.Forms.DateTimePicker();
            this.dateTimePicker_to = new System.Windows.Forms.DateTimePicker();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.button_tinhdoanhthu = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.textBox_doanhthu = new System.Windows.Forms.TextBox();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.button_phiguixe = new System.Windows.Forms.Button();
            this.comboBox_phithu = new System.Windows.Forms.ComboBox();
            this.label4 = new System.Windows.Forms.Label();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.SuspendLayout();
            // 
            // dateTimePicker_from
            // 
            this.dateTimePicker_from.CalendarFont = new System.Drawing.Font("Microsoft Sans Serif", 10.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.dateTimePicker_from.Format = System.Windows.Forms.DateTimePickerFormat.Short;
            this.dateTimePicker_from.ImeMode = System.Windows.Forms.ImeMode.NoControl;
            this.dateTimePicker_from.Location = new System.Drawing.Point(176, 80);
            this.dateTimePicker_from.MaxDate = new System.DateTime(2024, 4, 10, 0, 0, 0, 0);
            this.dateTimePicker_from.Name = "dateTimePicker_from";
            this.dateTimePicker_from.Size = new System.Drawing.Size(147, 27);
            this.dateTimePicker_from.TabIndex = 0;
            this.dateTimePicker_from.Value = new System.DateTime(2024, 4, 1, 0, 0, 0, 0);
            // 
            // dateTimePicker_to
            // 
            this.dateTimePicker_to.CalendarFont = new System.Drawing.Font("Microsoft Sans Serif", 10.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.dateTimePicker_to.Format = System.Windows.Forms.DateTimePickerFormat.Short;
            this.dateTimePicker_to.Location = new System.Drawing.Point(472, 80);
            this.dateTimePicker_to.MaxDate = new System.DateTime(2024, 4, 10, 0, 0, 0, 0);
            this.dateTimePicker_to.Name = "dateTimePicker_to";
            this.dateTimePicker_to.Size = new System.Drawing.Size(147, 27);
            this.dateTimePicker_to.TabIndex = 1;
            this.dateTimePicker_to.Value = new System.DateTime(2024, 4, 10, 0, 0, 0, 0);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.groupBox2);
            this.groupBox1.Controls.Add(this.groupBox3);
            this.groupBox1.Font = new System.Drawing.Font("Microsoft Sans Serif", 10.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.groupBox1.Location = new System.Drawing.Point(3, 1);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(796, 450);
            this.groupBox1.TabIndex = 2;
            this.groupBox1.TabStop = false;
            // 
            // groupBox2
            // 
            this.groupBox2.BackColor = System.Drawing.Color.LightGray;
            this.groupBox2.Controls.Add(this.dateTimePicker_from);
            this.groupBox2.Controls.Add(this.button_tinhdoanhthu);
            this.groupBox2.Controls.Add(this.dateTimePicker_to);
            this.groupBox2.Controls.Add(this.label1);
            this.groupBox2.Controls.Add(this.label3);
            this.groupBox2.Controls.Add(this.label6);
            this.groupBox2.Controls.Add(this.textBox_doanhthu);
            this.groupBox2.Location = new System.Drawing.Point(0, 11);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(796, 269);
            this.groupBox2.TabIndex = 12;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Doanh thu";
            // 
            // button_tinhdoanhthu
            // 
            this.button_tinhdoanhthu.Font = new System.Drawing.Font("Microsoft Sans Serif", 10.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.button_tinhdoanhthu.Location = new System.Drawing.Point(361, 120);
            this.button_tinhdoanhthu.Name = "button_tinhdoanhthu";
            this.button_tinhdoanhthu.Size = new System.Drawing.Size(75, 38);
            this.button_tinhdoanhthu.TabIndex = 7;
            this.button_tinhdoanhthu.Text = "OK";
            this.button_tinhdoanhthu.UseVisualStyleBackColor = true;
            this.button_tinhdoanhthu.Click += new System.EventHandler(this.button_tinhdoanhthu_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(171, 43);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(90, 25);
            this.label1.TabIndex = 3;
            this.label1.Text = "Từ ngày:";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label3.Location = new System.Drawing.Point(202, 183);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(108, 25);
            this.label3.TabIndex = 6;
            this.label3.Text = "Doanh thu:";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label6.Location = new System.Drawing.Point(467, 43);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(102, 25);
            this.label6.TabIndex = 4;
            this.label6.Text = "Đến ngày:";
            // 
            // textBox_doanhthu
            // 
            this.textBox_doanhthu.BackColor = System.Drawing.Color.White;
            this.textBox_doanhthu.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.textBox_doanhthu.Enabled = false;
            this.textBox_doanhthu.Font = new System.Drawing.Font("Microsoft Sans Serif", 16.2F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.textBox_doanhthu.ForeColor = System.Drawing.Color.Tomato;
            this.textBox_doanhthu.Location = new System.Drawing.Point(321, 175);
            this.textBox_doanhthu.Name = "textBox_doanhthu";
            this.textBox_doanhthu.Size = new System.Drawing.Size(172, 38);
            this.textBox_doanhthu.TabIndex = 5;
            this.textBox_doanhthu.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // groupBox3
            // 
            this.groupBox3.BackColor = System.Drawing.Color.LightGray;
            this.groupBox3.Controls.Add(this.button_phiguixe);
            this.groupBox3.Controls.Add(this.comboBox_phithu);
            this.groupBox3.Controls.Add(this.label4);
            this.groupBox3.Location = new System.Drawing.Point(0, 286);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(796, 158);
            this.groupBox3.TabIndex = 13;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Phí gửi xe";
            // 
            // button_phiguixe
            // 
            this.button_phiguixe.Location = new System.Drawing.Point(362, 98);
            this.button_phiguixe.Name = "button_phiguixe";
            this.button_phiguixe.Size = new System.Drawing.Size(80, 34);
            this.button_phiguixe.TabIndex = 11;
            this.button_phiguixe.Text = "OK";
            this.button_phiguixe.UseVisualStyleBackColor = true;
            this.button_phiguixe.Click += new System.EventHandler(this.button_phithu_Click);
            // 
            // comboBox_phithu
            // 
            this.comboBox_phithu.FormattingEnabled = true;
            this.comboBox_phithu.Items.AddRange(new object[] {
            "0",
            "5000",
            "10000",
            "15000",
            "20000",
            "25000",
            "30000"});
            this.comboBox_phithu.Location = new System.Drawing.Point(337, 46);
            this.comboBox_phithu.Name = "comboBox_phithu";
            this.comboBox_phithu.Size = new System.Drawing.Size(128, 28);
            this.comboBox_phithu.TabIndex = 10;
            this.comboBox_phithu.SelectedIndexChanged += new System.EventHandler(this.comboBox_phithu_SelectedIndexChanged);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label4.Location = new System.Drawing.Point(208, 45);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(103, 25);
            this.label4.TabIndex = 9;
            this.label4.Text = "Phí gửi xe:";
            // 
            // Doanhthu
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.groupBox1);
            this.MaximizeBox = false;
            this.Name = "Doanhthu";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Doanhthu";
            this.Load += new System.EventHandler(this.Doanhthu_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.DateTimePicker dateTimePicker_from;
        private System.Windows.Forms.DateTimePicker dateTimePicker_to;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox textBox_doanhthu;
        private System.Windows.Forms.ComboBox comboBox_phithu;
        private System.Windows.Forms.Button button_phiguixe;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Button button_tinhdoanhthu;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.GroupBox groupBox3;
    }
}
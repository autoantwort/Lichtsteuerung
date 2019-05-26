namespace WindowsInstaller
{
    partial class Form1
    {
        /// <summary>
        /// Erforderliche Designervariable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Verwendete Ressourcen bereinigen.
        /// </summary>
        /// <param name="disposing">True, wenn verwaltete Ressourcen gelöscht werden sollen; andernfalls False.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Vom Windows Form-Designer generierter Code

        /// <summary>
        /// Erforderliche Methode für die Designerunterstützung.
        /// Der Inhalt der Methode darf nicht mit dem Code-Editor geändert werden.
        /// </summary>
        private void InitializeComponent()
        {
            this.label1 = new System.Windows.Forms.Label();
            this.button_close = new System.Windows.Forms.Button();
            this.button_start = new System.Windows.Forms.Button();
            this.label_info_delete = new System.Windows.Forms.Label();
            this.label_info_copy = new System.Windows.Forms.Label();
            this.label_delete = new System.Windows.Forms.Label();
            this.label_copy = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label_wait = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(56, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Fortschritt:";
            // 
            // button_close
            // 
            this.button_close.Location = new System.Drawing.Point(41, 97);
            this.button_close.Name = "button_close";
            this.button_close.Size = new System.Drawing.Size(102, 23);
            this.button_close.TabIndex = 1;
            this.button_close.Text = "Beenden";
            this.button_close.UseVisualStyleBackColor = true;
            this.button_close.Click += new System.EventHandler(this.button_close_Click);
            // 
            // button_start
            // 
            this.button_start.Enabled = false;
            this.button_start.Location = new System.Drawing.Point(149, 97);
            this.button_start.Name = "button_start";
            this.button_start.Size = new System.Drawing.Size(131, 23);
            this.button_start.TabIndex = 2;
            this.button_start.Text = "Lichtsteuerung starten";
            this.button_start.UseVisualStyleBackColor = true;
            this.button_start.Click += new System.EventHandler(this.button2_Click);
            // 
            // label_info_delete
            // 
            this.label_info_delete.AutoSize = true;
            this.label_info_delete.ForeColor = System.Drawing.SystemColors.GrayText;
            this.label_info_delete.Location = new System.Drawing.Point(25, 47);
            this.label_info_delete.Name = "label_info_delete";
            this.label_info_delete.Size = new System.Drawing.Size(97, 13);
            this.label_info_delete.TabIndex = 3;
            this.label_info_delete.Text = "Alte Daten löschen";
            // 
            // label_info_copy
            // 
            this.label_info_copy.AutoSize = true;
            this.label_info_copy.ForeColor = System.Drawing.SystemColors.GrayText;
            this.label_info_copy.Location = new System.Drawing.Point(25, 69);
            this.label_info_copy.Name = "label_info_copy";
            this.label_info_copy.Size = new System.Drawing.Size(109, 13);
            this.label_info_copy.TabIndex = 4;
            this.label_info_copy.Text = "Neue Daten kopieren";
            // 
            // label_delete
            // 
            this.label_delete.AutoSize = true;
            this.label_delete.Location = new System.Drawing.Point(232, 47);
            this.label_delete.Name = "label_delete";
            this.label_delete.Size = new System.Drawing.Size(0, 13);
            this.label_delete.TabIndex = 5;
            // 
            // label_copy
            // 
            this.label_copy.AutoSize = true;
            this.label_copy.Location = new System.Drawing.Point(232, 69);
            this.label_copy.Name = "label_copy";
            this.label_copy.Size = new System.Drawing.Size(0, 13);
            this.label_copy.TabIndex = 6;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(25, 25);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(177, 13);
            this.label4.TabIndex = 7;
            this.label4.Text = "Auf Ende der Lichtsteuerung warten";
            // 
            // label_wait
            // 
            this.label_wait.AutoSize = true;
            this.label_wait.Location = new System.Drawing.Point(232, 25);
            this.label_wait.Name = "label_wait";
            this.label_wait.Size = new System.Drawing.Size(0, 13);
            this.label_wait.TabIndex = 8;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(288, 129);
            this.Controls.Add(this.label_wait);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label_copy);
            this.Controls.Add(this.label_delete);
            this.Controls.Add(this.label_info_copy);
            this.Controls.Add(this.label_info_delete);
            this.Controls.Add(this.button_start);
            this.Controls.Add(this.button_close);
            this.Controls.Add(this.label1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Name = "Form1";
            this.Text = "Lichtsteuerung Updater";
            this.Shown += new System.EventHandler(this.Form1_Shown);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button button_close;
        private System.Windows.Forms.Button button_start;
        private System.Windows.Forms.Label label_info_delete;
        private System.Windows.Forms.Label label_info_copy;
        private System.Windows.Forms.Label label_delete;
        private System.Windows.Forms.Label label_copy;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label_wait;
    }
}


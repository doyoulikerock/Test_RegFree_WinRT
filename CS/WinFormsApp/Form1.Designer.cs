namespace WinFormsApp
{
    partial class Form1
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
            this.Button1 = new System.Windows.Forms.Button();
            this.TextBox1 = new System.Windows.Forms.TextBox();
            this.button2 = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // Button1
            // 
            this.Button1.FlatAppearance.BorderColor = System.Drawing.Color.Red;
            this.Button1.Location = new System.Drawing.Point(123, 156);
            this.Button1.Margin = new System.Windows.Forms.Padding(5, 4, 5, 4);
            this.Button1.Name = "Button1";
            this.Button1.Size = new System.Drawing.Size(220, 72);
            this.Button1.TabIndex = 0;
            this.Button1.Text = "Call\r\n WinRT \r\nComponent";
            this.Button1.UseVisualStyleBackColor = true;
            this.Button1.Click += new System.EventHandler(this.Button1_Click);
            // 
            // TextBox1
            // 
            this.TextBox1.Location = new System.Drawing.Point(377, 180);
            this.TextBox1.Margin = new System.Windows.Forms.Padding(5, 4, 5, 4);
            this.TextBox1.Name = "TextBox1";
            this.TextBox1.Size = new System.Drawing.Size(614, 28);
            this.TextBox1.TabIndex = 1;
            this.TextBox1.TextChanged += new System.EventHandler(this.TextBox1_TextChanged);
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(123, 317);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(229, 49);
            this.button2.TabIndex = 2;
            this.button2.Text = "test WASAPI";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(10F, 18F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1080, 447);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.TextBox1);
            this.Controls.Add(this.Button1);
            this.Margin = new System.Windows.Forms.Padding(5, 4, 5, 4);
            this.Name = "Form1";
            this.Text = "Unpackaged WinForms App";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button Button1;
        private System.Windows.Forms.TextBox TextBox1;
        private System.Windows.Forms.Button button2;
    }
}


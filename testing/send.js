"use strict";
const nodemailer = require("nodemailer");

const IP = "172.23.89.237"
const PORT = 25

async function main() {
    // create reusable transporter object using the default SMTP transport
    const transporter = nodemailer.createTransport({
        host: IP,  // SMTP server hostname (localhost for local testing)
        port: PORT,           // SMTP server port (25 for unencrypted, 465 for SSL, 587 for TLS)
        secure: false,      // Use SSL/TLS (true for SSL, false for unencrypted)
        ignoreTLS: true,   // Ignore TLS support for testing purposes
    });

    // send mail with defined transport object
    let info = await transporter.sendMail({
        from: '"Fred Foo 👻" <foo@example.com>', // sender address
        to: "jeanmi@rf38.ddns.net", // list of receivers
        subject: "Hello ✔", // Subject line
        text: "Yoo it's a me !", // plain text body
    });

    console.log("Message sent: %s", info.messageId);
    // Message sent: <b658f8ca-6296-ccf4-8306-87d57a0b4321@example.com>

    // Preview only available when sending through an Ethereal account
    console.log("Preview URL: %s", nodemailer.getTestMessageUrl(info));
    // Preview URL: https://ethereal.email/message/WaQKMgKddxQDoou...
}

main().catch(console.error);
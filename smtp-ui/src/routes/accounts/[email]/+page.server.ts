import { readFile, readdir } from "node:fs/promises";

const BASE_MAIL = "C:/Users/Rafael/Desktop/Code/low-level/smtp/messages"

/** @type {import('./$types').PageLoad} */
export async function load({ params }) {
    if (params.email != "unknown" && !params.email.match(/^[\w-.]+@([\w-]+\.)+[\w-]{2,4}$/)) return {
        emails: []
    }

    const emails: string[] = [];

    const emailDates = await readdir(BASE_MAIL + "/" + params.email);
    for (const fileName of emailDates) {
        const emailBuffer = await readFile(BASE_MAIL + "/" + params.email + "/" + fileName);
        let raw = emailBuffer.toString("utf8");
        raw = raw.replace(/(?<=O:.+)\r?\n\r?\n/gm, "\n");

        // const parsed = await simpleParser(raw, {});

        emails.push(raw)
    }
    // console.log(emails[0].attachments[0])

    return {
        emails
    };
}
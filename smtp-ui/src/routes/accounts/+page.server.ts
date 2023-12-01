import { readdir, readFile } from "node:fs/promises";
import PostalMime, { type Email } from 'postal-mime';

const BASE_MAIL = "C:/Users/Rafael/Desktop/Code/low-level/smtp/messages"

/** @type {import('./$types').PageLoad} */
export async function load() {
    const accounts: {
        address: string, emails: {
            id: string;
            date: Date;
        }[]
    }[] = [];

    const accountsEmails = await readdir(BASE_MAIL);
    for (const addr of accountsEmails) {
        const emails = await readdir(BASE_MAIL + "/" + addr);
        const result = (await Promise.all(emails.map(async e => {
            try {
                let date = new Date(e.split(".")[0]?.replaceAll("\uf03a", ":"));
                const parser = new PostalMime();
                const content = await readFile(BASE_MAIL + "/" + addr + "/" + e);
                const parsed = await parser.parse(content);
                if (isNaN(date as unknown as number)) date = new Date();
                return {
                    id: parsed.messageId,
                    date: parsed.date ? new Date(parsed.date) : date
                }
            } catch (error) {
                return null
            }
        }))).filter(e => e);
        if (result.length == 0) continue;
        accounts.push({
            address: addr,
            emails: result
        })
    }

    return {
        accounts
    };
}
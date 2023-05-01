import { writable } from "svelte/store";


// Date of the emails
export const readEmails = writable<string[]>([])
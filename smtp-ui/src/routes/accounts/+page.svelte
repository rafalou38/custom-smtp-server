<script lang="ts">
	import { readEmails } from '../../lib/stores/emails';
	import { flip } from 'svelte/animate';
	import Icon from '@iconify/svelte';
	import type { PageData } from './$types';
	import { toRelativeTime } from '$lib/utils/time';
	import type { Email } from 'postal-mime';
	import { onMount } from 'svelte';

	export let data: PageData;
	const targetDate = new Date();

	let treated: typeof data.accounts = [];
	onMount(() => {
		treated = data.accounts;
		treated.sort((a, b) => {
			let aNearest = a.emails.reduce((a, b) => {
				return Math.abs(+b.date - +targetDate) < Math.abs(+a.date - +targetDate) ? b : a;
			});
			let bNearest = b.emails.reduce((a, b) => {
				return Math.abs(+b.date - +targetDate) < Math.abs(+a.date - +targetDate) ? b : a;
			});
			return +bNearest.date - +aNearest.date;
		});
	});

	let filter = '';
</script>

<div class="flex items-stretch flex-col font-[Poppins]">
	<input
		type="text"
		placeholder="Search an email"
		bind:value={filter}
		class="bg-zinc-200 border-zinc-300 focus:ring-transparent focus:outline-none placeholder:text-zinc-500 shadow-zinc-200 box-border p-2 m-2 text-black border rounded-sm shadow-sm"
	/>
	<ul>
		{#each treated as account (account.address)}
			{@const unread = account.emails.filter((e) => !$readEmails.includes(e.id)).length}
			{@const nearestDate = account.emails.reduce((a, b) =>
				Math.abs(+b.date - +targetDate) < Math.abs(+a.date - +targetDate) ? b : a
			)}

			<li animate:flip={{ duration: 300 }}>
				{#if account.address.includes(filter)}
					<a href="/accounts/{account.address}" class="item flex gap-2">
						<button
							class="aspect-square hover:bg-zinc-300 hover:border-zinc-400 flex items-center justify-center w-6 h-6 border border-transparent rounded"
						>
							<Icon icon="material-symbols:block" />
						</button>
						<div class="item grow flex justify-between">
							{account.address}
							<div class="">
								{toRelativeTime(nearestDate.date)}
								{#if unread == account.emails.length}
									<span class="font-bold">({account.emails.length})</span>
								{:else}
									<span >({account.emails.length})</span>
								{/if}
							</div>
						</div>
					</a>
				{/if}
			</li>
		{/each}
	</ul>
</div>

<style lang="postcss">
	li > .item {
		@apply w-full p-2 border border-transparent border-b-zinc-200 text-zinc-600 cursor-pointer ease-out  transition-all;
	}
	li > .item:hover {
		background: theme(backgroundColor.zinc.200);
		border: 1px solid theme(borderColor.zinc.300);
	}
</style>

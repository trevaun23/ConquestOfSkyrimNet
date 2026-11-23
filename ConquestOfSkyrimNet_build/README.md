# Conquest of SkyrimNet

Found your own faction in Skyrim, rule its holds, lead its armies and run its treasury, mostly by **talking to
people**. Conquest of SkyrimNet builds on Conquest of Skyrim and adds a government you speak to through
[SkyrimNet](https://github.com/MinLL/SkyrimNet-GamePlugin), along with a PrismaUI faction screen, requests from your
people, unrest and uprisings, army morale, deserters, bribery and coups, and settlements you build yourself.

> **New here?** Skip down to [What to say, and to whom](#what-to-say-and-to-whom). You don't need exact
> wording: say what you mean in your own words. The NPC decides whether to act, and anything that costs gold or
> can't be undone asks you to confirm first.

---

## Requirements

| Required | Link |
|---|---|
| Skyrim SE/AE with SKSE64 | [skse.silverlock.org](https://skse.silverlock.org/) |
| PapyrusUtil | [Nexus Mods](https://www.nexusmods.com/skyrimspecialedition/mods/13048) |
| SkyUI + MCM Helper | [SkyUI](https://www.nexusmods.com/skyrimspecialedition/mods/12604) · [MCM Helper](https://www.nexusmods.com/skyrimspecialedition/mods/53000) |
| **PrismaUI**: the faction screen | [Nexus Mods](https://www.nexusmods.com/skyrimspecialedition/mods/148718) |

| Optional / recommended | Link | What it adds |
|---|---|---|
| **SkyrimNet** (strongly recommended) | [GitHub](https://github.com/MinLL/SkyrimNet-GamePlugin) | Conversation, knowledge and memories. This guide assumes you have it. |
| **Physical Letters** | [GitHub](https://github.com/zevck/Physical-Letters) | Requests reach you as letters by courier. |
| **Holidays** | [Nexus Mods](https://www.nexusmods.com/skyrimspecialedition/mods/1533) | Festivals in your towns: an optional quest to attend, and a little loyalty when you do. |
| **SkyBuild** | [GitHub](https://github.com/Sokco/SkyBuild) | Only for the Settlements tab's **Update navmesh** button. Tick the installer's SkyBuild option (offered only when SkyBuild is installed) and load Conquest of SkyrimNet after SkyBuild. Everything else works without it. |
| **SeverActions** (recommended) | [GitHub](https://github.com/Severause/SeverActions) | More SkyrimNet actions, so NPCs can follow you, travel, craft, change outfits, make arrests and fight. It pairs well with the faction's officers and soldiers. |

Conquest of SkyrimNet **replaces** Conquest of Skyrim (`conquerskyrim.esp`), so remove that first.

## Install

1. Install `ConquestOfSkyrimNet_v<version>.zip` with your mod manager and enable `ConquestOfSkyrimNet.esp`.
2. In the installer, keep **SkyrimNet knowledge** ticked if you use SkyrimNet.
   If you use SkyBuild, tick **SkyBuild: Update navmesh button** too (it only appears when SkyBuild is installed), and
   set Conquest of SkyrimNet to load after SkyBuild.
3. Start a new game, or continue a save made with an earlier version of Conquest of SkyrimNet.
4. Read the **Start my Faction** book to pick your faction's name, army prefix and banner.
5. Go to MCM > Conquest of SkyrimNet > General and bind **Faction menu**. It's unbound by default.

## Your first hour

1. **Found your faction** with the book.
2. **Take a claim.** Clear a bandit camp, a mine or a fort and plant your banner (see [Taking claims](#taking-claims)).
3. **Appoint a Steward.** Ask a friendly NPC: *"I need someone to run my treasury. Will you be my Steward?"*
4. **Appoint a General**, and a **Captain** for your first army.
5. **Open the faction screen** with your hotkey. It shows your treasury, supplies, lands, armies and requests.
6. From then on, run the realm by talking to the people who serve you.

## Core features

**Conquer Skyrim.** Start with a single bandit camp and end up ruling Skyrim. Take any of 87 claims, then forts,
roads and whole holds, and storm cities against the Imperials and the Stormcloaks. Plan sieges at the war camp,
bring rams, ladders and catapults, and give the order to attack. If you'd rather pay than fight, a jarl at peace
with you may sell you their city or fort.

**Build your own settlements.** Plant a banner on open land and build a village from Skyrim and Hearthfire pieces:
homes, fields, mills, smithies, an inn, walls and watchtowers. Settlers move in, take the jobs, and send food, wood,
metal and gold home. Each settlement gets its own map marker for fast travel. Strong walls keep raiders out.

**Rule through conversation.** Appoint a Steward, a General, a Blacksmith, captains and jarls from the people you
meet, then give them orders out loud. "Lower the taxes in Riverwood." "Send Army 2 to attack Fort Greymoor."
"Recruit ten archers." With SkyrimNet, they know the real numbers, remember what you did, and talk back.

**Command armies that have feelings.** Every army has morale. Unpaid wages, empty bellies, long campaigns and
refused requests wear it down. Stars, victories and listening to your commanders lift it back up. Push an army too
far and it **deserts**: it becomes a free company that holds your fort, raids your stores and tempts other armies to
join it. You can crush them, pay them off or recognise their realm.

**Keep your people loyal, or face uprisings.** Hunger, heavy taxes, no garrison and ignored court cases make towns
restless. Unrest turns into an **uprising**: armed townsfolk take the town, stop paying taxes, and eventually open
the gates to your enemies. You can meet their demands, kill their leader or march in a garrison. Each choice has a
cost.

**Rebels fight back.** Risen towns and free companies take their own turn in the war. Every day, when the economy
updates, they may storm one of your cities in a full Conquest of Skyrim battle. You can defend it yourself or trust
your garrison.

**Topple jarls from within.** In enemy cities, bribe the guards, fund the nobles and win over the citizens, either
in person or through your Steward's agents. When support is high enough, launch a **coup**. Torch-bearing crowds
gather, you confront the jarl, and the guards decide whose side they're on.

**Hear from your people.** Towns, officers, commanders and Thanes send you requests (Thanes pitch private
ventures that pay you back with profit): festivals, tax relief, trade, upgrades,
or a famous army to guard their walls. With Physical Letters they arrive by courier. Your answers change loyalty
and morale.

**Run an economy.** Gold, food, metal and wood come from your towns, mines, farms and settlements. Set taxes,
trade with your towns and rival factions, upgrade barracks, farms, mills and mines, and buy supplies straight from
farmers, miners and lumberjacks.

**Shape your faction.** Choose your faction's name, banner, uniforms, troop mix and your own title (Supreme
Leader, High Priest, or anything you like). Write its culture and military
doctrine, and pick how it's led: laissez-faire, democratic or an iron-fisted dictatorship. Everyone in Skyrim
learns who you are.

**Follow it all in your journal.** Unrest, uprisings, deserters and coups each get a quest with stages and map
markers. The faction screen shows everything else at a glance.

---

## What to say, and to whom

The lines below are only **examples**. SkyrimNet understands what you mean, so phrase things however you like.
Who you talk to matters: each office can do certain things.

### Anyone (appointments)

| You want to… | Say something like… |
|---|---|
| Make them your **General** | "Will you be the General of my army?" |
| Make them your **Steward** | "I need a Steward to keep my treasury and trade. Will you take the post?" |
| Make them your **Blacksmith** | "Will you be my faction's Blacksmith and see to the troops' gear?" |
| Make them your **Court Wizard** | "I'd like you to serve as my Court Wizard." |
| Make them **Jarl** of a city or **leader** of a village you rule | "Whiterun needs a jarl. Will you rule it for me?" · "Be Riverwood's leader." |
| Make them **steward of a hold** (one town, not your faction's Steward) | "Will you be the steward of the Rift?" · "Run Riften's affairs as its steward." |
| Make them **housecarl of a hold** | "Will you be housecarl of Whiterun hold and guard its jarl?" |
| Make them a **Thane** (1,000 gold) | "I name you Thane of Whiterun." |
| Make them **Captain** of an army | "Will you lead one of my armies as Captain?" |
| **Dismiss** someone | "You're relieved of your post." |
| Have them **change clothes** (officers) | "Wear something different." · "Wear outfit template 2." |

Anyone holding an office may also ask to step down. You can accept or refuse.

### Your Steward (treasury, trade, taxes, towns)

| You want to… | Say something like… |
|---|---|
| Deposit gold | "Put 5,000 gold into the treasury." |
| Withdraw gold | "I need 2,000 gold from the treasury." |
| Look over the books | "Let me see the ledger." · "Show me the full economy." |
| Change a town's taxes (0–50%) | "Lower the taxes in Riverwood to 10%." · "Raise Whiterun's taxes to 30%." |
| Trade with your own towns | "Export 10 food to Riverwood." · "Import 5 wood from Whiterun." · "Stop the metal trade with Dawnstar." |
| Trade with the Imperials or Stormcloaks | "Draw up a trade with the Stormcloaks: our wood for their metal." |
| Upgrade buildings | "Upgrade the barracks in Falkreath." · "Improve the mines in Markarth." |
| Hire guards | "Hire more guards for Riverwood." |
| Declare war | "Prepare a declaration of war against the Imperials." |
| Bribe an enemy city's guards (paid from the treasury) | "Have our agents bribe the guards of Solitude." |
| Fund an enemy city's nobles | "Fund the nobles of Markarth." |
| Start a coup | "Tell your agents to plan the coup on Riften." |
| Found a settlement where you stand | "Let's found a settlement here." |
| Build in a settlement | "I want to build something here." |
| Calm a town in unrest | "Tell Riverwood we'll meet their demands." |

A town's **own jarl or steward** can do the tax, trade, upgrade and guard jobs for *their* town. Your faction's
Steward can do them for any town.

### Your General (armies, war, stars)

| You want to… | Say something like… |
|---|---|
| See every army | "Let me look over the war table." |
| Pass an order to a commander | "Tell Captain Aerina to garrison Whiterun." · "Have Army 2 return home." |
| Camp an army in a hold | "Have Army 3 make camp in the Reach." |
| Plan an attack | "Let's make war camp plans for the Pale." |
| Order an attack directly | "Send Army 1 and Army 2 to attack Fort Greymoor." |
| Begin or call off the attack | "It's time to begin the attack." · "Call off the attack." |
| Recruit troops | "Recruit 5 archers and 10 soldiers for Army 5." |
| Write a troop template | "Put 10 soldiers, 7 archers and 2 crossbowmen in template 1." |
| Apply a template | "Apply template 1 to Army 3." |
| Award a star (10,000 gold, 100 food) | "Give Captain Aerina a star for her service." |
| Remove a commander | "Remove Captain Hrolf from command." |
| Name an army | "Name Army 2 the Iron Wolves, sworn to free the Reach." |
| Put down deserters | "Send the Iron Wolves against the deserters at Fort Sungard." |
| Plan a coup through your agents | "Tell your agents to plan the coup on Riften." |
| Put down an uprising | "Send Army 2 to crush the rising in Shor's Stone." |
| Found or build a settlement | "Found a settlement here." · "Open the build menu." |

### Captains and Commanders (your own army)

| You want to… | Say something like… |
|---|---|
| March with you | "Follow me." · "March with me." |
| Formation | "Form up." · "Break formation." |
| Gather food | "Take the army hunting." |
| Go home | "Take your army back home." |
| Garrison | "Garrison Whiterun." · "Hold Fort Greymoor." |
| Patrol or camp | "Make camp in Falkreath hold." |
| Plan or begin an attack | "Let's make war camp plans." · "Begin the attack." |
| Uniform | "Your army needs a uniform." · "Wear outfit template 3." |
| Name the army | "Call your army the Iron Wolves." |
| Award a star | "I'm awarding you a star." |
| Check morale | "How are the troops holding up?" |
| Fight deserters | "Take your army and crush those deserters." |
| Put down an uprising | "Take your army and deal with the rebels in Riften." |

Captains and Commanders know their army's morale and what's behind it, so ask them.

### Your Blacksmith (gear and recruits)

| You want to… | Say something like… |
|---|---|
| Set troop outfits | "Let's sort out the troops' equipment." · "Give the archers their own outfit." |
| Better armour | "Can we get the troops into steel armour?" |
| Who can be recruited | "Let me choose which races can join the army." |
| Templates | "Put 8 warriors and 4 crossbowmen in template 2." · "Apply template 2 to Army 1." |
| Army uniform | "Make a uniform for Army 4." |

### Jarls and leaders of *other* sides (while at peace)

Each seller names their own price (at least 500,000 gold for a hold, 80,000 for a fort, often with food, metal
and wood on top) and sticks to it unless something changes their mind. A jarl may even hand it over for free:
under duress, to repay a great favour, or as part of a partnership. That's entirely their call.

| You want to… | Say something like… |
|---|---|
| Ask their price | "What's your price for Falkreath?" (they name it and remember it) |
| Buy their city | "What would it take for Falkreath to join my faction? I can pay." |
| Buy their hold's fort | "Sell me Fort Neugrad. Gold, food, whatever you need." |
| Buy a fort from Tullius or Ulfric | "General Tullius, I'll buy Fort Hraggstad from the Legion." |

### Merchants, farmers, miners and lumberjacks (supplies)

| You want to… | Say something like… |
|---|---|
| Buy food from farm folk | "I'll buy 30 food for my faction." |
| Buy metal from miners | "Sell me 20 ore for my army." |
| Buy wood from mill workers | "I need 40 wood. Can you spare it?" |
| Sell your surplus to people of means | "My faction has spare wood. Want to buy 50?" |

Farm households (such as the Battle-Borns) and people who buy wheat for favours sell **food**. Miners and mine
owners sell **metal**, and mill and lumber workers sell **wood**. Merchants sell all three.

### Guards, nobles and citizens of enemy cities (intrigue)

| You want to… | Say something like… |
|---|---|
| Bribe a guard (from your purse) | "Take this gold. Remember who your friends are when the time comes." |
| Fund a noble, merchant or citizen | "Here's gold for you and your friends. Your jarl doesn't deserve you." |
| Persuade them (free, once a day each) | "Your jarl is bleeding this city dry. You know it." |
| Launch the coup (once support is 40+) | "It's time. Help me take the jarl down." |

### Rebels and deserters

| Who | Say something like… |
|---|---|
| A **rebel leader** (or the town's jarl/steward) | "I hear you. You'll have gold, food and lower taxes. Lay down your arms." |
| A **free company** leader | "I'll pay your arrears. Swear back in." · "Keep what you hold. I'll recognise your realm." |

### Someone who sent you a request

Requests from your towns, armies and officers appear on the Requests tab, and as letters if you use Physical
Letters. Answer them on the tab or face to face:

- "Yes, you have my approval." (accept)
- "No, not this time." (deny)
- "Let's meet in the middle." (half the sum for half the loyalty)

### Settlers and town leaders

| You want to… | Say something like… |
|---|---|
| Build here | "Let's build a farmhouse." · "Open the build menu." |
| Ask how it's going | "How is the settlement doing? Are we safe from raids?" |

---

## Taking claims

A **claim** is a small holding of your own outside the towns, such as a bandit camp, a cave, a tower, a mine or a
farm. Claims are how a new faction gets started. Each one you hold opens another **army slot** for a Captain,
and the first one becomes your base of operations.

1. **Find a claimable place.** The [appendix](#appendix-every-claim) lists all 87.
2. **Clear it.** Kill every enemy there: bandits, Forsworn, necromancers, warlocks, giants, Thalmor and bears.
   If anyone is left, you're told how many.
3. **Plant your banner.** Stand in the place itself, where its name shows when you arrive (outside, for farms and
   forts), and use the **Place Banner** power.
   Your faction's banner goes up, and the claim appears on the faction screen (Lands tab > Claims).
4. **Manage it.** Activate the banner to open the holding menu:
   - **Hire guard** (250 gold, 1 food a day, up to 5): defends the claim.
   - **Hire miner** (mines only; 400 gold, 1 food a day, up to 5): +2 metal a day each.
   - **Hire farmhand** (farms only; 400 gold, up to 5): +2 food a day each (each grows 3 and eats 1).
   - **Set as base of operations**: your officers gather here.

Notes:
- Prices follow MCM > Economy.
- You can hold up to 62 claims at once.
- Army slots stop at 30.
- Each claim's army slot shows on the Officers tab. Pick a Captain there, or ask someone in conversation.

## The faction screen (hotkey)

The screen reopens on the last tab you used. It always has **Overview, Requests, Officers, Armies, Lands, Unrest,
Settlements, Culture** and **Settings**. Some tabs appear only when you open the screen near the right officer:

- **Steward** tab: near your Steward, a town steward or one of your jarls.
- **War Table** tab: near your General.
- **Blacksmith** tab: near your Blacksmith.

## Tips for new players

- **Speak naturally.** You don't need the exact phrases above. Name the town, army or person you mean.
- **You stay in control.** Anything with a cost or lasting effect (gold, wars, appointments, attacks, coups) shows
  a confirmation. **"Decline (silent)"** cancels it without the NPC mentioning it.
- **Watch loyalty.** Hungry, over-taxed or unguarded towns grow restless, then rise up. Unpaid, hungry or
  long-deployed armies lose morale and may desert.
- **Give stars** to good commanders. A star gives +15 morale at once and stronger troops.
- **Ask your officers.** Your Steward knows the treasury and towns, your General knows every army, and Captains
  know their own troops.
- **Check your journal.** Unrest, deserters and coups each get a quest with stages.

## Troubleshooting

- **The hotkey does nothing:** bind it in the MCM and found your faction first.
- **An NPC agrees but nothing happens:** make sure they hold the right office (see the tables above) and that
  SkyrimNet is running. Turn on MCM > Debug > Debug logging and check `ConquestOfSkyrimNet.log`.
- **No requests:** MCM > Requests must be on, and a town has to need something. Letters take a day to arrive.
- **No Steward / War Table tab:** stand closer to that officer (MCM > General > Officer reach).

The full in-game guide is in `Docs/ConquestOfSkyrimNet/UserGuide.md`. The changes in each version are listed in
`CHANGELOG.md`.

## Appendix: every claim

**Hideouts** take guards. **Mines** take guards and miners (metal). **Farms** take guards and farmhands (food).
Claims marked *(new)* were added by Conquest of SkyrimNet.

| Hold | Claim | Type |
|---|---|---|
| Haafingar | Broken Oar Grotto | Hideout |
| Haafingar | Ironback Hideout | Hideout |
| Haafingar | Northwatch Keep *(new)* | Hideout |
| Haafingar | Orphan's Tear | Hideout |
| Haafingar | Pinemoon Cave | Hideout |
| Hjaalmarch | Eldersblood Peak | Hideout |
| Hjaalmarch | Movarth's Lair | Hideout |
| Hjaalmarch | Orotheim | Hideout |
| Hjaalmarch | Robber's Gorge | Hideout |
| Hjaalmarch | Skyborn Altar | Hideout |
| Hjaalmarch | Stonehill Bluff | Hideout |
| Hjaalmarch | Talking Stone Camp | Hideout |
| The Pale | Blizzard Rest | Hideout |
| The Pale | Red Road Pass | Hideout |
| The Pale | Shearpoint | Hideout |
| The Pale | Tumble Arch Pass | Hideout |
| Winterhold | Fort Fellhammer | Mine |
| Winterhold | Hob's Fall Cave | Hideout |
| Winterhold | Mount Anthor | Hideout |
| Winterhold | Snowpoint Beacon | Hideout |
| Winterhold | The Pride of Tel Vos | Hideout |
| Winterhold | Winter War | Hideout |
| The Reach | Broken Tower Redoubt | Hideout |
| The Reach | Bruca's Leap Redoubt | Hideout |
| The Reach | Dragon Bridge Overlook | Hideout |
| The Reach | Dragontooth Crater | Hideout |
| The Reach | Druadach Redoubt | Hideout |
| The Reach | Four Skull Lookout | Hideout |
| The Reach | Hag Rock Redoubt | Hideout |
| The Reach | Karthspire | Hideout |
| The Reach | Kolskeggr Mine | Mine |
| The Reach | Red Eagle Redoubt | Hideout |
| Whiterun | Bleakwind Basin | Hideout |
| Whiterun | Broken Fang Cave | Hideout |
| Whiterun | Fellglow Keep | Hideout |
| Whiterun | Graywinter Watch | Hideout |
| Whiterun | Guldun Rock | Hideout |
| Whiterun | Halted Stream Camp | Mine |
| Whiterun | Redoran's Retreat | Hideout |
| Whiterun | Serpent's Bluff Redoubt | Hideout |
| Whiterun | Silent Moons Camp | Hideout |
| Whiterun | Sleeping Tree Camp | Hideout |
| Whiterun | Swindler's Den | Hideout |
| Whiterun | Valtheim Keep | Hideout |
| Whiterun | White River Watch | Hideout |
| Eastmarch | Bonestrewn Crest | Hideout |
| Eastmarch | Broken Limb Camp | Hideout |
| Eastmarch | Cradlecrush Rock | Hideout |
| Eastmarch | Cragslane Cavern | Hideout |
| Eastmarch | Cragwallow Slope | Hideout |
| Eastmarch | Lost Knife Hideout | Hideout |
| Eastmarch | Mara's Eye Pond | Hideout |
| Eastmarch | Mistwatch | Hideout |
| Eastmarch | Morvunskar | Hideout |
| Eastmarch | Steamcrag Camp | Hideout |
| Eastmarch | Stony Creek Cave | Hideout |
| Eastmarch | Traitor's Post | Hideout |
| Eastmarch | Uttering Hills Camp | Hideout |
| Falkreath | Ancient's Ascent | Hideout |
| Falkreath | Bannermist Tower | Hideout |
| Falkreath | Bilegulch Mine | Mine |
| Falkreath | Bloodlet Throne | Hideout |
| Falkreath | Cracked Tusk Keep | Hideout |
| Falkreath | Embershard | Mine |
| Falkreath | Falkreath Watchtower | Hideout |
| Falkreath | Helgen | Hideout |
| Falkreath | Knifepoint Ridge | Mine |
| Falkreath | Secunda's Shelf | Hideout |
| Falkreath | Shriekwind Bastion | Hideout |
| The Rift | Alchemist's Shack *(new)* | Hideout |
| The Rift | Autumnwatch Tower | Hideout |
| The Rift | Boulderfall Cave | Hideout |
| The Rift | Broken Helm Hollow | Hideout |
| The Rift | Bthalft | Hideout |
| The Rift | Darklight Tower | Hideout |
| The Rift | Faldar's Tooth | Hideout |
| The Rift | Honeystrand Cave *(new)* | Hideout |
| The Rift | Lost Prospect Mine *(new)* | Mine |
| The Rift | Lost Tongue Overlook | Hideout |
| The Rift | Nilheim | Hideout |
| The Rift | Northwind Mine *(new)* | Mine |
| The Rift | Northwind Summit | Hideout |
| The Rift | Redwater Den | Hideout |
| The Rift | Rift Watchtower | Hideout |
| The Rift | Sarethi Farm *(new)* | Farm |
| The Rift | Treva's Watch | Hideout |
| Solstheim | Fort Frostmoth *(new)* | Hideout |

## Credits

Built on **Conquest of Skyrim**. Conversation through [SkyrimNet](https://github.com/MinLL/SkyrimNet-GamePlugin). Menus by
[PrismaUI](https://www.nexusmods.com/skyrimspecialedition/mods/148718). Letters by [Physical Letters](https://github.com/zevck/Physical-Letters).
Recommended companion: [SeverActions](https://github.com/Severause/SeverActions).

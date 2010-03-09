Lookup subclass: TraitLookup [
	lookup: selector in: searchClass forMethod: senderMethod [
		| methods composition methodDictionary |
		searchClass isNil ifTrue:[^#()].
		methods := self lookup: selector in: searchClass superclass forMethod: senderMethod.		
		composition := searchClass traitComposition.
		composition ifNotNil:
			[| composedMethods |
			composedMethods := composition lookup lookup: selector in: composition forMethod: senderMethod.			
			composedMethods ifNotNil:
				[methods := composedMethods , methods ]].
						
		(methodDictionary := searchClass methodDictionary) ifNotNil:
			[(methodDictionary includesKey: selector) ifTrue:
				[methods := (Array with: (searchClass methodDictionary at: selector)) , methods]].
		^methods
	]		
]

Object subclass: TraitA [
    methodA [
    	^#traitA
	]
]

Object subclass: MockA [    

]	

Eval [
	MockA traitComposition: TraitA
]

MockA subclass: MockB [
	methodA [
		^#mockB
	]
]

MockB subclass: MockC [

]

TestCase subclass: MOPTraitsTest [
	| mockA mockB mockC |
	
	setUp [		
		mockA := MockA new.
		mockB := MockB new.
		mockC := MockC new.
		Behavior flushCache.
		'==' printNl.
	]
	
	tearDown [
		MockA lookup: nil.
	]
	
	test_01 [
		self assert: MockA traitComposition == TraitA.
	
		self should: [MockA new methodA] raise: Error.		
		self assert: MockB new methodA = #mockB.
		
		
		MockA lookup: TraitLookup new.
		
		self assert: MockA new methodA = #traitA.
		
		self assert: MockB new methodA = #mockB.		
	]		
	
						
]
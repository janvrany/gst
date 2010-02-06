Object subclass: MockA [    
    public [
    	<access: #public>
    	'public' printNl.
    	^#public		  
	]
	
	private [
    	<access: #private>
    	'private' printNl.
    	^#private		  
	]    
	
	protected [
    	<access: #protected>
    	'protected' printNl.
    	^#protected		  
	]
	
	callPrivate [
		^self private
	]
	
	callProtected [
		^self protected
	]			
]

MockA subclass: MockB [
	callPrivateB [
		^self private
	]
	
	callProtectedB [
		^self protected
	]			
]

CompiledMethod extend [
	access [ ^(self attributeAt: #access: ifAbsent:[^#public]) argument	]	
	isPublic [ ^self access == #public ]
	isPrivate [ ^self access == #private ]
	isProtected [ ^self access == #protected ]		
]

Lookup subclass: PublicPrivateProtectedLookup [
	keep: receiverMethod forSendFrom: senderMethodClass [
		| access |
		access := receiverMethod access.
		access == #public ifTrue:[^true].
		access == #protected ifTrue:[^senderMethodClass includesBehavior: receiverMethod methodClass].
		access == #private ifTrue:[^senderMethodClass == receiverMethod methodClass].
	]


	lookup: selector in: initialSearchClass forMethod: senderMethod [
		| senderMethodClass |
		senderMethodClass := senderMethod methodClass.
		^(Lookup builtin lookup: selector in: initialSearchClass forMethod: senderMethod)		
			select:[:method|self keep: method forSendFrom: senderMethodClass].						  
	]
]

TestCase subclass: MOPPublicPrivateProtectedTest [
	| mockA mockB |
	
	setUp [
		MockA lookup: nil.
		mockA := MockA new.
		mockB := MockB new.
		Behavior flushCache.
		'==' printNl.
	]
	
	test_public [							
		self assert: mockA public == #public.
		self assert: mockB public == #public.
		
		'--' printNl.
		MockA lookup: PublicPrivateProtectedLookup new.
		
		self assert: mockA public    = #public.					
		self assert: mockB public    = #public.			
	]
	
	test_protected [							
		self assert: mockA protected     == #protected.
		self assert: mockA callProtected == #protected.
		self assert: mockB protected     == #protected.
		self assert: mockB callProtectedB == #protected.
		
		'--' printNl.
		MockA lookup: PublicPrivateProtectedLookup new.

		self should:[ mockA protected ] raise: Error.
		self assert: mockA callProtected == #protected.
		self should:[ mockB protected ] raise: Error.
		self assert: mockB callProtectedB == #protected.		
	]
	
	test_private [							
		self assert: mockA private     == #private.
		self assert: mockA callPrivate == #private.
		self assert: mockB private     == #private.
		self assert: mockB callPrivate == #private.
		
		'--' printNl.
		MockA lookup: PublicPrivateProtectedLookup new.

		self should:[ mockA private ] raise: Error.
		self assert: mockA callPrivate == #private.
		self should:[ mockB private ] raise: Error.
		self should:[ mockB callPrivateB ] raise: Error.		
	]	
						
]